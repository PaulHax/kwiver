// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/**
 * \file
 * \brief Implementation of basic camera and landmark initialization algorithm
 */

#include "initialize_cameras_landmarks_basic.h"

#include <deque>
#include <iterator>

#include <vital/exceptions.h>
#include <vital/io/eigen_io.h>

#include <vital/algo/bundle_adjust.h>
#include <vital/algo/estimate_essential_matrix.h>
#include <vital/algo/optimize_cameras.h>
#include <vital/algo/triangulate_landmarks.h>
#include <vital/types/vector.h>

#include <arrows/core/match_matrix.h>
#include <arrows/mvg/algo/triangulate_landmarks.h>
#include <arrows/mvg/epipolar_geometry.h>
#include <arrows/mvg/metrics.h>
#include <arrows/mvg/necker_reverse.h>
#include <arrows/mvg/transform.h>
#include <arrows/mvg/triangulate.h>

using namespace kwiver::vital;

namespace {

// detect bad tracks
std::set< track_id_t >
detect_bad_tracks(
  const camera_map::map_camera_t& cams,
  const landmark_map::map_landmark_t& lms,
  const std::vector< track_sptr >& trks,
  double error_tol = 5.0 )
{
  typedef landmark_map::map_landmark_t lm_map_t;

  std::set< track_id_t > to_remove;
  for( const lm_map_t::value_type& p : lms )
  {
    landmark_map::map_landmark_t lm_single;
    lm_single.insert( p );

    double rmse = kwiver::arrows::mvg::reprojection_rmse(
      cams, lm_single,
      trks );
    if( rmse > error_tol )
    {
      to_remove.insert( p.first );
    }
  }
  return to_remove;
}

// remove landmarks with IDs in the set
void
remove_landmarks(
  const std::set< track_id_t >& to_remove,
  landmark_map::map_landmark_t& lms )
{
  for( const track_id_t& tid : to_remove )
  {
    lms.erase( tid );
  }
}

// remove landmarks with IDs in the set
void
remove_tracks(
  const std::set< track_id_t >& to_remove,
  std::vector< track_sptr >& trks )
{
  std::vector< track_sptr > kept_tracks;
  for( const track_sptr& t : trks )
  {
    const track_id_t& tid = t->id();
    if( !to_remove.count( tid ) )
    {
      kept_tracks.push_back( t );
    }
  }
  trks.swap( kept_tracks );
}

} // namespace

namespace kwiver {

namespace arrows {

namespace mvg {

// Private implementation class
class initialize_cameras_landmarks_basic::priv
{
public:
  // Constructor
  priv( initialize_cameras_landmarks_basic& parent )
    : parent( parent )
  {}

  initialize_cameras_landmarks_basic& parent;

  // Destructor
  ~priv()
  {}

  // Construct and initialized camera for \a frame
  camera_sptr init_camera(
    frame_id_t frame, frame_id_t last_frame,
    const camera_map::map_camera_t& cams,
    const std::vector< track_sptr >& trks,
    const landmark_map::map_landmark_t& lms ) const;

  // Re-triangulate all landmarks for provided tracks
  void retriangulate(
    landmark_map::map_landmark_t& lms,
    const camera_map::map_camera_t& cams,
    const std::vector< track_sptr >& trks,
    const std::set< landmark_id_t >& new_lm_ids ) const;

  // Estimate the translation scale using a 2d-3d correspondence
  double estimate_t_scale(
    const vector_3d& KRp,
    const vector_3d& Kt,
    const vector_2d& pt2d ) const;

  // Pass through this callback to another callback but cache the return value
  bool pass_through_callback(
    callback_t cb,
    camera_map_sptr cams,
    landmark_map_sptr lms,
    feature_track_set_changes_sptr track_changes );

  // Configuration values
  bool
  c_verbose() const { return parent.c_verbose; }
  bool
  c_init_from_last() const { return parent.c_init_from_last; }
  bool
  c_retriangulate_all() const { return parent.c_retriangulate_all; }
  double
  c_reverse_ba_error_ratio() const { return parent.c_reverse_ba_error_ratio; }
  unsigned int
  c_next_frame_max_distance() const { return parent.c_next_frame_max_distance; }
  double
  c_global_ba_rate() const { return parent.c_global_ba_rate; }
  double
  c_interim_reproj_thresh() const { return parent.c_interim_reproj_thresh; }
  double
  c_final_reproj_thresh() const { return parent.c_final_reproj_thresh; }
  double
  c_zoom_scale_thresh() const { return parent.c_zoom_scale_thresh; }

  // processing classes
  vital::algo::estimate_essential_matrix_sptr
  d_essential_mat_estimator() const { return parent.c_essential_mat_estimator; }
  vital::algo::optimize_cameras_sptr
  d_camera_optimizer() const { return parent.c_camera_optimizer; }
  vital::algo::triangulate_landmarks_sptr
  d_lm_triangulator() const { return parent.c_lm_triangulator; }
  vital::algo::bundle_adjust_sptr
  d_bundle_adjuster() const { return parent.c_bundle_adjuster; }
  vital::camera_intrinsics_sptr
  d_base_camera() const { return parent.c_base_camera; }

  bool continue_processing;
  vital::simple_camera_perspective m_base_camera;
  // Logger handle
  vital::logger_handle_t m_logger;
};

// ----------------------------------------------------------------------------
// Construct and initialized camera for \a frame
camera_sptr
initialize_cameras_landmarks_basic::priv
::init_camera(
  frame_id_t frame, frame_id_t last_frame,
  const camera_map::map_camera_t& cams,
  const std::vector< track_sptr >& trks,
  const landmark_map::map_landmark_t& lms ) const
{
  typedef landmark_map::map_landmark_t lm_map_t;

  // extract corresponding image points and landmarks
  std::vector< vector_2d > pts_right, pts_left;
  std::vector< landmark_sptr > pts_lm;
  for( unsigned int i = 0; i < trks.size(); ++i )
  {
    auto frame_data = std::dynamic_pointer_cast< feature_track_state >(
      *( trks[ i ]->find( frame ) ) );
    auto last_frame_data = std::dynamic_pointer_cast< feature_track_state >(
      *( trks[ i ]->find( last_frame ) ) );
    if( !frame_data || !last_frame_data )
    {
      continue;
    }
    pts_right.push_back( last_frame_data->feature->loc() );
    pts_left.push_back( frame_data->feature->loc() );

    lm_map_t::const_iterator li = lms.find( trks[ i ]->id() );
    if( li != lms.end() )
    {
      pts_lm.push_back( li->second );
    }
    else
    {
      pts_lm.push_back( landmark_sptr() );
    }
  }

  // compute the essential matrix from the corresponding points
  camera_map::map_camera_t::const_iterator ci = cams.find( last_frame );
  if( ci == cams.end() )
  {
    VITAL_THROW( invalid_value, "Camera for last frame not provided." );
  }

  auto prev_cam = std::dynamic_pointer_cast< camera_perspective >( ci->second );
  camera_intrinsics_sptr cal_right = prev_cam->intrinsics();
  const camera_intrinsics_sptr cal_left = m_base_camera.get_intrinsics();
  std::vector< bool > inliers;
  essential_matrix_sptr E_sptr = d_essential_mat_estimator()->estimate(
    pts_right, pts_left,
    cal_right, cal_left,
    inliers, 2.0 );
  const essential_matrix_d E( *E_sptr );

  unsigned num_inliers = static_cast< unsigned >( std::count(
    inliers.begin(),
    inliers.end(), true ) );
  if( this->c_verbose() )
  {
    LOG_INFO(
      m_logger, "E matrix num inliers = " << num_inliers
                                          << "/" << inliers.size() );
  }

  // get the first inlier index
  unsigned int inlier_idx = 0;
  for(; inlier_idx < inliers.size() && !inliers[ inlier_idx ]; ++inlier_idx ) {}

  // get the first inlier correspondence to
  // disambiguate essential matrix solutions
  vector_2d left_pt = cal_left->unmap( pts_left[ inlier_idx ] );
  vector_2d right_pt = cal_right->unmap( pts_right[ inlier_idx ] );

  // compute the corresponding camera rotation and translation (up to scale)
  vital::simple_camera_perspective cam =
    extract_valid_left_camera( E, left_pt, right_pt );
  cam.set_intrinsics( cal_left );

  // compute the scale from existing landmark locations (if available)
  matrix_3x3d prev_R( prev_cam->rotation().matrix() );
  vector_3d prev_t = prev_cam->translation();
  matrix_3x3d R = cam.get_rotation().matrix();
  vector_3d t = cam.translation();
  std::vector< double > scales;
  scales.reserve( num_inliers );
  for( unsigned int i = 0; i < inliers.size(); ++i )
  {
    if( !inliers[ i ] || !pts_lm[ i ] )
    {
      continue;
    }

    vector_3d pt3d = prev_R * pts_lm[ i ]->loc() + prev_t;
    const vector_2d& pt2d = cal_left->unmap( pts_left[ i ] );
    scales.push_back( estimate_t_scale( R * pt3d, t, pt2d ) );
  }

  // find the median scale
  double median_scale = 1.0;
  if( !scales.empty() )
  {
    size_t n = scales.size() / 2;
    std::nth_element( scales.begin(), scales.begin() + n, scales.end() );
    median_scale = scales[ n ];
  }
  if( this->c_verbose() )
  {
    LOG_DEBUG(m_logger, "median scale = " << median_scale);
    if( !scales.empty() )
    {
      std::sort( scales.begin(), scales.end() );
      LOG_DEBUG(
        m_logger, "min/max scale = " << scales.front()
                                     << "/" << scales.back() );
    }
  }

  // adjust pose relative to the previous camera
  vector_3d new_t = cam.get_rotation() * prev_cam->translation() +
                    median_scale * cam.translation();
  cam.set_rotation( cam.get_rotation() * prev_cam->rotation() );
  cam.set_translation( new_t );

  return cam.clone();
}

// ----------------------------------------------------------------------------
// Re-triangulate all landmarks for provided tracks
void
initialize_cameras_landmarks_basic::priv
::retriangulate(
  landmark_map::map_landmark_t& lms,
  const camera_map::map_camera_t& cams,
  const std::vector< track_sptr >& trks,
  const std::set< landmark_id_t >& new_lm_ids ) const
{
  typedef landmark_map::map_landmark_t lm_map_t;

  lm_map_t init_lms;
  for( const track_sptr& t : trks )
  {
    const track_id_t& tid = t->id();
    if( !this->c_retriangulate_all() &&
        new_lm_ids.find( tid ) == new_lm_ids.end() )
    {
      continue;
    }

    lm_map_t::const_iterator li = lms.find( tid );
    if( li == lms.end() )
    {
      auto lm = std::make_shared< landmark_d >( vector_3d( 0, 0, 0 ) );
      init_lms[ static_cast< landmark_id_t >( tid ) ] = lm;
    }
    else
    {
      init_lms.insert( *li );
    }
  }

  landmark_map_sptr lm_map =
    std::make_shared< simple_landmark_map >( init_lms );
  camera_map_sptr cam_map = std::make_shared< simple_camera_map >( cams );
  auto tracks = std::make_shared< feature_track_set >( trks );
  this->d_lm_triangulator()->triangulate( cam_map, tracks, lm_map );

  // detect and remove landmarks with large triangulation error
  std::set< track_id_t > to_remove = detect_bad_tracks(
    cams,
    lm_map->landmarks(),
    trks, c_interim_reproj_thresh() );
  for( const lm_map_t::value_type& p : lm_map->landmarks() )
  {
    lms[ p.first ] = p.second;
  }
  LOG_INFO(
    m_logger, "removing " << to_remove.size()
                          << "/" << lm_map->size()
                          << " landmarks with RMSE > "
                          << c_interim_reproj_thresh() );
  remove_landmarks( to_remove, lms );
}

// Estimate the translation scale using a 2d-3d correspondence
double
initialize_cameras_landmarks_basic::priv
::estimate_t_scale(
  const vector_3d& KRp,
  const vector_3d& Kt,
  const vector_2d& pt2d ) const
{
  vector_3d a = KRp;
  vector_3d b = Kt;
  a.x() = pt2d.x() * a.z() - a.x();
  b.x() = pt2d.x() * b.z() - b.x();
  a.y() = pt2d.y() * a.z() - a.y();
  b.y() = pt2d.y() * b.z() - b.y();

  double cx = a.x() * b.z() - a.z() * b.x();
  double cy = a.y() * b.z() - a.z() * b.y();
  return ( a.x() * cx + a.y() * cy ) / -( b.x() * cx + b.y() * cy );
}

// Pass through this callback to another callback but cache the return value
bool
initialize_cameras_landmarks_basic::priv
::pass_through_callback(
  callback_t cb,
  camera_map_sptr cams,
  landmark_map_sptr lms,
  feature_track_set_changes_sptr track_changes )
{
  this->continue_processing = cb( cams, lms, track_changes );
  return this->continue_processing;
}

// Constructor
void
initialize_cameras_landmarks_basic
::initialize()
{
  KWIVER_INITIALIZE_UNIQUE_PTR( priv, d_ );
  attach_logger( "arrows.mvg.initialize_cameras_landmarks_basic" );
  d_->m_logger = logger();
  // use the MVG triangulation as the default, users can change it
  c_lm_triangulator = std::make_shared< mvg::triangulate_landmarks >();
}

// Destructor
initialize_cameras_landmarks_basic
::~initialize_cameras_landmarks_basic()
{}

// ----------------------------------------------------------------------------
// Set this algorithm's properties via a config block
void
initialize_cameras_landmarks_basic
::set_configuration_internal( vital::config_block_sptr config )
{
  const camera_intrinsics_sptr K = d_->m_base_camera.get_intrinsics();

  if( c_bundle_adjuster && this->m_callback )
  {
    using std::placeholders::_1;
    using std::placeholders::_2;
    using std::placeholders::_3;

    callback_t pcb =
      std::bind(
        &initialize_cameras_landmarks_basic::priv::pass_through_callback,
        d_.get(), this->m_callback, _1, _2, _3 );
    c_bundle_adjuster->set_callback( pcb );
  }

  vital::config_block_sptr bc = config->subblock( "base_camera" );
  simple_camera_intrinsics K2( bc->get_value< double >(
    "focal_length",
    K->focal_length() ),
    bc->get_value< vector_2d >(
      "principal_point",
      K->principal_point() ),
    bc->get_value< double >(
      "aspect_ratio",
      K->aspect_ratio() ),
    bc->get_value< double >(
      "skew",
      K->skew() ) );
  d_->m_base_camera.set_intrinsics( K2.clone() );
}

// ----------------------------------------------------------------------------
// Check that the algorithm's currently configuration is valid
bool
initialize_cameras_landmarks_basic
::check_configuration( vital::config_block_sptr config ) const
{
  if( config->get_value< std::string >( "camera_optimizer", "" ) != "" &&
      !vital::check_nested_algo_configuration< vital::algo::optimize_cameras >(
        "camera_optimizer", config ) )
  {
    return false;
  }
  if( config->get_value< std::string >( "bundle_adjuster", "" ) != "" &&
      !vital::check_nested_algo_configuration< vital::algo::bundle_adjust >(
        "bundle_adjuster", config ) )
  {
    return false;
  }
  return vital::check_nested_algo_configuration< vital::algo::estimate_essential_matrix >(
    "essential_mat_estimator",
    config ) &&
         vital::check_nested_algo_configuration< vital::algo::triangulate_landmarks >( "lm_triangulator", config );
}

namespace {

// Extract valid cameras and cameras to initialize.

/**
 * If \a cameras is NULL then return empty cam_map and frame_ids unchanged.
 * If not NULL, return frame_ids containing IDs of all NULL cameras and
 * return cam_map containing all valid cameras.
 * \param [in]     cameras the camera map object to extract from
 * \param [in,out] frame_ids the set of all frames (input),
 *                           the set of frame to initialize (output)
 * \param [out]    cam_map the extract map of valid camera
 */
void
extract_cameras(
  const camera_map_sptr& cameras,
  std::set< frame_id_t >& frame_ids,
  camera_map::map_camera_t& cam_map )
{
  cam_map.clear();
  if( !cameras )
  {
    return;
  }

  typedef camera_map::map_camera_t map_cam_t;

  map_cam_t all_cams = cameras->cameras();

  // Find the set of all cameras that need to be initialized
  std::set< frame_id_t > new_frames;
  for( const map_cam_t::value_type& p : all_cams )
  {
    if( p.second )
    {
      cam_map.insert( p );
    }
    else if( frame_ids.count( p.first ) )
    {
      new_frames.insert( p.first );
    }
  }
  frame_ids = new_frames;
}

// ----------------------------------------------------------------------------

// Extract valid landmarks and landmarks to initialize.

/**
 * If \a landmarks is NULL then return empty lm_map and track_ids unchanged.
 * If not NULL, return track_ids containing IDs of all NULL landmark and
 * return lm_map containing all valid landmarks.
 * \param [in]     landmarks the landmark map object to extract from
 * \param [in,out] track_ids the set of all tracks (input),
 *                           the set of landmarks to initialize (output)
 * \param [out]    lm_map the extract map of valid landmarks
 */
void
extract_landmarks(
  const landmark_map_sptr& landmarks,
  std::set< track_id_t >& track_ids,
  landmark_map::map_landmark_t& lm_map )
{
  lm_map.clear();
  if( !landmarks )
  {
    return;
  }

  typedef landmark_map::map_landmark_t map_landmark_t;

  map_landmark_t all_lms = landmarks->landmarks();

  // Find the set of all landmarks that need to be initialized
  std::set< track_id_t > new_landmarks;
  for( const map_landmark_t::value_type& p : all_lms )
  {
    if( p.second )
    {
      lm_map.insert( p );
    }
    else if( track_ids.count( p.first ) )
    {
      new_landmarks.insert( p.first );
    }
  }
  track_ids = new_landmarks;
}

// ----------------------------------------------------------------------------
// Find the closest frame number with an existing camera
frame_id_t
find_closest_camera(
  const frame_id_t& frame,
  const camera_map::map_camera_t& cams )
{
  typedef vital::camera_map::map_camera_t map_cam_t;

  frame_id_t other_frame = cams.rbegin()->first;
  map_cam_t::const_iterator ci = cams.lower_bound( frame );
  if( ci == cams.end() )
  {
    other_frame = cams.rbegin()->first;
  }
  else
  {
    other_frame = ci->first;
    if( ci != cams.begin() &&
        ( other_frame - frame ) >= ( frame - ( --ci )->first ) )
    {
      other_frame = ci->first;
    }
  }
  return other_frame;
}

// Find the subset of new_frames within dist frames of a camera in cams
std::set< frame_id_t >
find_nearby_new_frames(
  const std::set< frame_id_t >& new_frames,
  const camera_map::map_camera_t& cams,
  unsigned int dist )
{
  std::set< frame_id_t > nearby;
  for( camera_map::map_camera_t::value_type p : cams )
  {
    frame_id_t f = p.first < dist ? 0 : p.first - dist;
    for(; f < p.first + dist; ++f )
    {
      nearby.insert( f );
    }
  }

  std::set< frame_id_t > new_nearby;
  std::set_intersection(
    nearby.begin(), nearby.end(),
    new_frames.begin(), new_frames.end(),
    std::inserter( new_nearby, new_nearby.begin() ) );
  return new_nearby;
}

// ----------------------------------------------------------------------------
// find the best pair of camera indices to start with
void
find_best_initial_pair(
  const Eigen::SparseMatrix< unsigned int >& mm,
  int& i, int& j,
  vital::logger_handle_t& logger )
{
  typedef Eigen::Matrix< unsigned int, Eigen::Dynamic, 1 > vectorXu;

  const int cols = mm.cols();
  const vectorXu d = mm.diagonal();

  // compute the maximum off-diagonal value
  unsigned int global_max_matches = 0;
  for( int k = 0; k < cols; ++k )
  {
    for( Eigen::SparseMatrix< unsigned int >::InnerIterator it( mm, k ); it;
         ++it )
    {
      if( it.row() > k && it.value() > global_max_matches )
      {
        global_max_matches = it.value();
      }
    }
  }

  const unsigned int threshold = std::max( global_max_matches / 2, 20u );

  LOG_DEBUG(logger, "global max " << global_max_matches);
  LOG_DEBUG(logger, "threshold " << threshold);
  for( int x = 1; x < cols; ++x )
  {
    unsigned int max_matches = 0;
    int max_i = 0, max_j = 0;
    for( int y = 0; y < cols - x; ++y )
    {
      unsigned int matches = mm.coeff( x + y, y );
      if( matches > max_matches )
      {
        max_matches = matches;
        max_i = y;
        max_j = x + y;
      }
    }
    LOG_DEBUG(
      logger, "max matches at " << x << " is " << max_matches
                                << " at " << max_i << ", " << max_j);
    if( max_matches < threshold )
    {
      break;
    }
    i = max_i;
    j = max_j;
  }
}

// ----------------------------------------------------------------------------
// find the frame in the set \p new_frame_ids that sees the most
// landmarks in \p lms in the track set \p tracks.
frame_id_t
next_best_frame(
  const track_set_sptr tracks,
  const vital::landmark_map::map_landmark_t& lms,
  const std::set< frame_id_t >& new_frame_ids,
  vital::logger_handle_t& logger )
{
  const std::vector< track_sptr > trks = tracks->tracks();
  typedef std::map< frame_id_t, unsigned int > frame_map_t;

  frame_map_t vis_count;
  for( const track_sptr& t : trks )
  {
    if( lms.find( t->id() ) != lms.end() )
    {
      const std::set< frame_id_t > t_frames = t->all_frame_ids();
      for( const frame_id_t& fid : t_frames )
      {
        if( new_frame_ids.find( fid ) == new_frame_ids.end() )
        {
          continue;
        }

        frame_map_t::iterator fmi = vis_count.find( fid );
        if( fmi == vis_count.end() )
        {
          vis_count.insert( std::pair< frame_id_t, unsigned int >( fid, 1 ) );
        }
        else
        {
          ++fmi->second;
        }
      }
    }
  }
  // check if remaining new frames see no existing landmarks
  if( vis_count.empty() )
  {
    LOG_INFO(logger, "remaining frames do not see any existing landmarks" );
    return *new_frame_ids.begin();
  }

  // find the maximum observation
  unsigned int max_count = 0;
  frame_id_t best_frame = 0;
  for( const frame_map_t::value_type& p : vis_count )
  {
    if( p.second > max_count )
    {
      max_count = p.second;
      best_frame = p.first;
    }
  }
  LOG_DEBUG(
    logger, "frame " << best_frame << " sees "
                     << max_count << " landmarks" );
  return best_frame;
}

// ----------------------------------------------------------------------------
double
estimate_gsd(
  const frame_id_t frame,
  std::vector< track_sptr > const& tracks,
  vital::landmark_map::map_landmark_t const& lms )
{
  std::vector< vector_3d > pts_3d;
  std::vector< vector_2d > pts_2d;
  for( track_sptr const& t : tracks )
  {
    auto const& lm_itr = lms.find( t->id() );
    if( lm_itr != lms.end() )
    {
      auto const& ts_itr = t->find( frame );
      if( ts_itr != t->end() )
      {
        auto fts = std::dynamic_pointer_cast< feature_track_state >( *ts_itr );
        if( fts && fts->feature )
        {
          pts_3d.push_back( lm_itr->second->loc() );
          pts_2d.push_back( fts->feature->loc() );
        }
      }
    }
  }

  std::vector< double > gsds;
  for( unsigned int i = 1; i < pts_3d.size(); ++i )
  {
    for( unsigned int j = 0; j < i; ++j )
    {
      const double dist_3d = ( pts_3d[ i ] - pts_3d[ j ] ).norm();
      const double dist_2d = ( pts_2d[ i ] - pts_2d[ j ] ).norm();
      if( dist_2d > 0.0 )
      {
        const double gsd = dist_3d / dist_2d;
        gsds.push_back( gsd );
      }
    }
  }
  if( gsds.size() == 0 )
  {
    return 0.0;
  }
  // compute the median GSD
  std::nth_element( gsds.begin(), gsds.begin() + gsds.size() / 2, gsds.end() );
  return gsds[ gsds.size() / 2 ];
}

} // end anonymous namespace

// ----------------------------------------------------------------------------
// Initialize the camera and landmark parameters given a set of tracks
void
initialize_cameras_landmarks_basic
::initialize(
  camera_map_sptr& cameras,
  landmark_map_sptr& landmarks,
  feature_track_set_sptr tracks,
  sfm_constraints_sptr constraints ) const
{
  if( !tracks )
  {
    VITAL_THROW( invalid_value, "Some required input data is NULL." );
  }
  if( !c_essential_mat_estimator )
  {
    VITAL_THROW( invalid_value, "Essential matrix estimator not initialized." );
  }
  if( !c_lm_triangulator )
  {
    VITAL_THROW( invalid_value, "Landmark triangulator not initialized." );
  }
  typedef vital::camera_map::map_camera_t map_cam_t;
  typedef vital::landmark_map::map_landmark_t map_landmark_t;

  // Extract the existing cameras and camera ids to be initialized
  std::set< frame_id_t > frame_ids = tracks->all_frame_ids();
  map_cam_t cams;
  extract_cameras( cameras, frame_ids, cams );

  std::set< frame_id_t > new_frame_ids( frame_ids );

  // Extract the existing landmarks and landmark ids to be initialized
  std::set< track_id_t > track_ids = tracks->all_track_ids();
  map_landmark_t lms;
  extract_landmarks( landmarks, track_ids, lms );

  std::set< landmark_id_t > new_lm_ids( track_ids.begin(), track_ids.end() );

  std::vector< track_sptr > trks = tracks->tracks();

  if( new_frame_ids.empty() && new_lm_ids.empty() )
  {
    // nothing to initialize
    return;
  }

  // initialize landmarks if there are already at least two cameras
  if( cams.size() > 2 && !new_lm_ids.empty() )
  {
    map_landmark_t init_lms;
    for( const landmark_id_t& lmid : new_lm_ids )
    {
      landmark_sptr lm( new landmark_d( vector_3d( 0, 0, 0 ) ) );
      init_lms[ static_cast< landmark_id_t >( lmid ) ] = lm;
    }

    landmark_map_sptr lm_map( new simple_landmark_map( init_lms ) );
    camera_map_sptr cam_map( new simple_camera_map( cams ) );
    c_lm_triangulator->triangulate( cam_map, tracks, lm_map );

    for( const map_landmark_t::value_type& p : lm_map->landmarks() )
    {
      lms[ p.first ] = p.second;
    }
  }

  std::vector< frame_id_t > mm_frames( frame_ids.begin(), frame_ids.end() );
  Eigen::SparseMatrix< unsigned int > mm = match_matrix( tracks, mm_frames );
  int init_i = 0, init_j = 0;
  find_best_initial_pair( mm, init_i, init_j, d_->m_logger );
  LOG_INFO(
    logger(), "Initializing with frames " << mm_frames[ init_i ]
                                          << " and " << mm_frames[ init_j ]);

  if( cams.empty() )
  {
    // first frame, initialize to base camera
    frame_id_t f = mm_frames[ init_i ];
    new_frame_ids.erase( f );
    cams[ f ] = d_->m_base_camera.clone();
  }

  // keep track of the number of cameras needed for the next bundle adjustment
  size_t num_cams_for_next_ba = 2;
  if( c_global_ba_rate > 1.0 )
  {
    while( num_cams_for_next_ba < cams.size() )
    {
      num_cams_for_next_ba =
        static_cast< size_t >( std::ceil(
          c_global_ba_rate *
          num_cams_for_next_ba ) );
    }
  }

  // keep track of if we've tried a Necker revseral, only do it once.
  bool tried_necker_reverse = false;
  d_->continue_processing = true;
  while( !new_frame_ids.empty() && d_->continue_processing )
  {
    frame_id_t f;
    if( cams.size() == 1 )
    {
      f = mm_frames[ init_j ];
    }
    else
    {
      unsigned int search_range = c_next_frame_max_distance;
      if( search_range < 1 )
      {
        f = next_best_frame( tracks, lms, new_frame_ids, d_->m_logger );
      }
      else
      {
        std::set< frame_id_t > nearby;
        const frame_id_t max_frame = tracks->last_frame();
        while( nearby.empty() && search_range < max_frame )
        {
          nearby = find_nearby_new_frames( new_frame_ids, cams, search_range );
          search_range *= 2;
        }
        f = next_best_frame( tracks, lms, nearby, d_->m_logger );
      }
    }
    new_frame_ids.erase( f );

    // get the closest frame number with an existing camera
    frame_id_t other_frame = find_closest_camera( f, cams );
    if( c_verbose )
    {
      LOG_DEBUG(logger(), "frame " << f << " uses reference " << other_frame);
    }

    // get the subset of tracks that have features on frame f
    auto ftracks = std::make_shared< feature_track_set >(
      tracks->active_tracks( static_cast< int >( f ) ) );

    // find existing landmarks for tracks also having features on the other
    // frame
    map_landmark_t flms;
    std::vector< track_sptr > aftracks =
      ftracks->active_tracks( static_cast< int >( other_frame ) );
    for( const track_sptr& t : aftracks )
    {
      map_landmark_t::const_iterator li = lms.find( t->id() );
      if( li != lms.end() )
      {
        flms.insert( *li );
      }
    }

    // test for a large scale change
    double scale_change = 1.0;
    if( flms.size() > 1 )
    {
      double gsd_prev = estimate_gsd( other_frame, trks, flms );
      double gsd_next = estimate_gsd( f, trks, flms );
      scale_change = gsd_prev / gsd_next;
      LOG_DEBUG(
        logger(), "GSD estimates: " << gsd_prev << ", "
                                    << gsd_next << " ratio "
                                    << scale_change);
      // small scale changes are less likely to be zoom, so share intrinsics
      if( scale_change < 1.0 + c_zoom_scale_thresh &&
          1.0 / scale_change < 1.0 + c_zoom_scale_thresh )
      {
        scale_change = 1.0;
      }
    }

    if( c_init_from_last && c_camera_optimizer && flms.size() > 3 )
    {
      auto cam_ptr =
        std::dynamic_pointer_cast< camera_perspective >( cams[ other_frame ] );
      cams[ f ] = cam_ptr->clone();
    }
    else if( trks.size() > 10 )
    {
      cams[ f ] = d_->init_camera( f, other_frame, cams, trks, flms );
    }
    else
    {
      break;
    }

    if( scale_change != 1.0 )
    {
      // construct a new camera a new intrinsic model.
      auto cam_ptr =
        std::dynamic_pointer_cast< camera_perspective >( cams[ f ] );
      auto K =
        std::make_shared< simple_camera_intrinsics >( *cam_ptr->intrinsics() );
      K->set_focal_length( K->get_focal_length() * scale_change );
      cams[ f ] = std::make_shared< simple_camera_perspective >(
        cam_ptr->center(), cam_ptr->rotation(), K );
      LOG_DEBUG(logger(), "Constructing new intrinsics" );
    }

    // optionally optimize the new camera
    if( c_camera_optimizer && flms.size() > 3 )
    {
      camera_map::map_camera_t opt_cam_map;
      opt_cam_map[ f ] = cams[ f ];

      camera_map_sptr opt_cams =
        std::make_shared< simple_camera_map >( opt_cam_map );
      landmark_map_sptr landmarks_map =
        std::make_shared< simple_landmark_map >( flms );
      auto f_tracks = std::make_shared< feature_track_set >( trks );
      c_camera_optimizer->optimize(
        opt_cams, f_tracks, landmarks_map,
        constraints );
      cams[ f ] = opt_cams->cameras()[ f ];
    }

    // triangulate (or re-triangulate) points seen by the new camera
    d_->retriangulate( lms, cams, trks, new_lm_ids );

    if( c_verbose )
    {
      camera_map::map_camera_t new_cam_map;
      new_cam_map[ f ] = cams[ f ];

      std::vector< double > rpe = reprojection_errors( new_cam_map, lms, trks );
      if( rpe.empty() )
      {
        LOG_DEBUG(logger(), "no landmark projections for new camera" );
      }
      else
      {
        std::sort( rpe.begin(), rpe.end() );
        LOG_DEBUG(
          logger(), "new camera reprojections - median: "
            << rpe[ rpe.size() / 2 ] << " max: " << rpe.back() );
      }
    }

    if( c_bundle_adjuster && cams.size() >= num_cams_for_next_ba )
    {
      LOG_INFO(
        logger(), "Running Global Bundle Adjustment on "
          << cams.size() << " cameras and "
          << lms.size() << " landmarks" );
      num_cams_for_next_ba =
        static_cast< size_t >( std::ceil(
          c_global_ba_rate *
          num_cams_for_next_ba ) );

      camera_map_sptr ba_cams( new simple_camera_map( cams ) );
      landmark_map_sptr ba_lms( new simple_landmark_map( lms ) );
      double init_rmse = reprojection_rmse( cams, lms, trks );
      LOG_INFO(logger(), "initial reprojection RMSE: " << init_rmse);

      c_bundle_adjuster->optimize( ba_cams, ba_lms, tracks, constraints );
      cams = ba_cams->cameras();
      lms = ba_lms->landmarks();
      if( !d_->continue_processing )
      {
        break;
      }

      // detect tracks/landmarks with large error and remove them
      std::set< track_id_t > to_remove = detect_bad_tracks(
        cams, lms, trks,
        c_interim_reproj_thresh );
      LOG_INFO(
        logger(), "removing " << to_remove.size()
                              << "/" << lms.size()
                              << " landmarks with RMSE > "
                              << c_interim_reproj_thresh);
      remove_landmarks( to_remove, lms );

      std::vector< track_sptr > all_trks = tracks->tracks();
      remove_tracks( to_remove, all_trks );
      tracks = std::make_shared< feature_track_set >( all_trks );

      double final_rmse = reprojection_rmse( cams, lms, trks );
      LOG_INFO(logger(), "final reprojection RMSE: " << final_rmse);

      auto cam_ptr =
        std::dynamic_pointer_cast< camera_perspective >( cams.begin()->second );
      LOG_DEBUG(
        logger(), "updated focal length "
          << cam_ptr->intrinsics()->focal_length() );

      if( !tried_necker_reverse && c_reverse_ba_error_ratio > 0 )
      {
        // reverse cameras and optimize again
        camera_map_sptr ba_cams2( new simple_camera_map( cams ) );
        landmark_map_sptr ba_lms2( new simple_landmark_map( lms ) );
        necker_reverse( ba_cams2, ba_lms2 );
        c_lm_triangulator->triangulate( ba_cams2, tracks, ba_lms2 );
        init_rmse = reprojection_rmse(
          ba_cams2->cameras(),
          ba_lms2->landmarks(), trks );
        LOG_DEBUG(
          logger(),
          "Necker reversed initial reprojection RMSE: " << init_rmse);
        if( init_rmse < final_rmse * c_reverse_ba_error_ratio )
        {
          // Only try a Necker reversal once when we have enough data to
          // support it. We will either decide to reverse or not.
          // Either way we should not have to try this again.
          tried_necker_reverse = true;
          LOG_INFO(
            logger(),
            "Running Necker reversed bundle adjustment for comparison" );
          c_bundle_adjuster->optimize( ba_cams2, ba_lms2, tracks, constraints );

          map_cam_t cams2 = ba_cams2->cameras();
          map_landmark_t lms2 = ba_lms2->landmarks();
          double final_rmse2 = reprojection_rmse( cams2, lms2, trks );
          LOG_DEBUG(
            logger(),
            "Necker reversed final reprojection RMSE: " << final_rmse2);

          if( final_rmse2 < final_rmse )
          {
            LOG_INFO(logger(), "Necker reversed solution is better" );
            cams = ba_cams2->cameras();
            lms = ba_lms2->landmarks();
          }
        }
      }
    }

    if( c_verbose )
    {
      camera_map_sptr ba_cams( new simple_camera_map( cams ) );
      landmark_map_sptr ba_lms( new simple_landmark_map( lms ) );
      double curr_rmse = reprojection_rmse( cams, lms, trks );
      LOG_INFO(logger(), "current reprojection RMSE: " << curr_rmse);

      LOG_DEBUG(
        logger(),
        "frame " << f << " - num landmarks = " << lms.size() );
    }
    if( this->m_callback )
    {
      d_->continue_processing = this->m_callback(
        std::make_shared< simple_camera_map >( cams ),
        std::make_shared< simple_landmark_map >( lms ),
        nullptr );
    }
  }

  // Run a final bundle adjustment
  if( c_bundle_adjuster && d_->continue_processing )
  {
    LOG_INFO(logger(), "Running final bundle adjustment" );

    camera_map_sptr ba_cams( new simple_camera_map( cams ) );
    landmark_map_sptr ba_lms( new simple_landmark_map( lms ) );
    double init_rmse = reprojection_rmse( cams, lms, trks );
    LOG_DEBUG(logger(), "initial reprojection RMSE: " << init_rmse);

    c_bundle_adjuster->optimize( ba_cams, ba_lms, tracks, constraints );

    map_cam_t cams1 = ba_cams->cameras();
    map_landmark_t lms1 = ba_lms->landmarks();
    double final_rmse1 = reprojection_rmse( cams1, lms1, trks );
    LOG_DEBUG(logger(), "final reprojection RMSE: " << final_rmse1);

    double final_med_err = reprojection_median_error( cams1, lms1, trks );
    LOG_DEBUG(logger(), "final reprojection Median Error: " << final_med_err);
    cams = ba_cams->cameras();
    lms = ba_lms->landmarks();

    // if using bundle adjustment, remove landmarks with large error
    // after optimization
    const double outlier_thresh = final_med_err * c_final_reproj_thresh;
    std::set< track_id_t > to_remove = detect_bad_tracks(
      cams, lms, trks,
      outlier_thresh );
    LOG_INFO(
      logger(), "removing " << to_remove.size()
                            << "/" << lms.size()
                            << " landmarks with RMSE > "
                            << outlier_thresh);
    remove_landmarks( to_remove, lms );
  }
  cameras = camera_map_sptr( new simple_camera_map( cams ) );
  landmarks = landmark_map_sptr( new simple_landmark_map( lms ) );
}

// ----------------------------------------------------------------------------
// Set a callback function to report intermediate progress
void
initialize_cameras_landmarks_basic
::set_callback( callback_t cb )
{
  vital::algo::initialize_cameras_landmarks::set_callback( cb );
  // pass callback on to bundle adjuster if available
  if( c_bundle_adjuster )
  {
    using std::placeholders::_1;
    using std::placeholders::_2;
    using std::placeholders::_3;

    callback_t pcb =
      std::bind(
        &initialize_cameras_landmarks_basic::priv::pass_through_callback,
        d_.get(), cb, _1, _2, _3 );
    c_bundle_adjuster->set_callback( pcb );
  }
}

} // end namespace mvg

} // end namespace arrows

} // end namespace kwiver
