// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Implementation of Ceres bundle adjustment algorithm

#include "bundle_adjust.h"

#include <vital/io/eigen_io.h>
#include <vital/vital_config.h>

#include <arrows/ceres/options.h>
#include <arrows/ceres/reprojection_error.h>
#include <arrows/ceres/types.h>

#include <ceres/ceres.h>
#include <ceres/loss_function.h>

#include <iomanip>
#include <iostream>
#include <set>
#include <unordered_set>

using namespace kwiver::vital;

namespace kwiver {

namespace arrows {

namespace ceres {

// ----------------------------------------------------------------------------
// Private implementation class
class bundle_adjust::priv
{
public:
  // Constructor

public:
  priv( bundle_adjust& parent )
    : parent( parent ),
      ceres_callback( this )
  {}

  bundle_adjust& parent;

  // the input cameras to update in place
  camera_map::map_camera_t cams;
  // the input landmarks to update in place
  landmark_map::map_landmark_t lms;
  // a map from track id to landmark parameters
  std::unordered_map< track_id_t, std::vector< double > > landmark_params;
  // a map from frame number to extrinsic parameters
  std::unordered_map< frame_id_t, std::vector< double > > camera_params;
  // vector of unique camera intrinsic parameters
  std::vector< std::vector< double > > camera_intr_params;
  // a map from frame number to index of unique camera intrinsics in
  // camera_intr_params
  std::unordered_map< frame_id_t, unsigned int > frame_to_intr_map;

  // --------------------------------------------------------------------------
  // A class to register callbacks with Ceres
  class StateCallback
    : public ::ceres::IterationCallback
  {
  public:
    explicit StateCallback( bundle_adjust::priv* p )
      : parent( p ) {}

    ::ceres::CallbackReturnType
    operator()( const ::ceres::IterationSummary& summary )
    {
      if( !parent )
      {
        return ::ceres::SOLVER_CONTINUE;
      }

      bundle_adjust& ba = parent->parent;
      if( ba.c_verbose )
      {
        if( summary.iteration == 0 )
        {
          LOG_DEBUG(
            ba.logger(),
            "iter         cost  cost_change   |gradient|       "
            "|step|  iter_time total_time" );
        }
        LOG_DEBUG(
          ba.logger(),
          std::setw( 4 ) << summary.iteration << " " <<
            std::setw( 12 ) << summary.cost << " " <<
            std::setw( 12 ) << summary.cost_change << " " <<
            std::setw( 12 ) << summary.gradient_max_norm << " " <<
            std::setw( 12 ) << summary.step_norm << " " <<
            std::setw( 10 ) << summary.iteration_time_in_seconds << " " <<
            std::setw( 10 ) << summary.cumulative_time_in_seconds );
      }
      return ( ba.trigger_callback() )
             ? ::ceres::SOLVER_CONTINUE
             : ::ceres::SOLVER_TERMINATE_SUCCESSFULLY;
    }

    bundle_adjust::priv* parent;
  };

  // the ceres callback class
  StateCallback ceres_callback;
};

// ----------------------------------------------------------------------------
void
bundle_adjust
::initialize()
{
  KWIVER_INITIALIZE_UNIQUE_PTR( priv, d_ );
  attach_logger( "arrows.ceres.bundle_adjust" );
}

// ----------------------------------------------------------------------------
// Check that the algorithm's currently configuration is valid
bool
bundle_adjust
::check_configuration( VITAL_UNUSED config_block_sptr config ) const
{
  std::string msg;
  if( c_solver_options->options.IsValid( &msg ) )
  {
    LOG_ERROR( logger(), msg );
    return false;
  }
  return true;
}

void
bundle_adjust
::set_configuration_internal( vital::config_block_sptr )
{
  if( c_solver_options )
  {
    c_solver_options->options.callbacks.push_back( &( d_->ceres_callback ) );
  }
}

class distance_constraint
{
public:
  /// Constructor
  distance_constraint( const double distance_squared )
    : distance_squared_( distance_squared ) {}

  template < typename T >
  bool
  operator()(
    const T* const pose_0,
    const T* const pose_1,
    T* residuals ) const
  {
    const T* center_0 = pose_0 + 3;
    const T* center_1 = pose_1 + 3;
    T dx = center_0[ 0 ] - center_1[ 0 ];
    T dy = center_0[ 1 ] - center_1[ 1 ];
    T dz = center_0[ 2 ] - center_1[ 2 ];

    T dist = dx * dx + dy * dy + dz * dz;

    residuals[ 0 ] = ( dist - distance_squared_ );

    return true;
  }

  /// Cost function factory
  static ::ceres::CostFunction*
  create( const double distance )
  {
    typedef distance_constraint Self;
    return new ::ceres::AutoDiffCostFunction< Self, 1, 6,
      6 >( new Self( distance ) );
  }

  double distance_squared_;
};

/// Optimize the camera and landmark parameters given a set of tracks
void
bundle_adjust
::optimize(
  camera_map_sptr& cameras,
  landmark_map_sptr& landmarks,
  feature_track_set_sptr tracks,
  sfm_constraints_sptr constraints ) const
{
  simple_camera_perspective_map cams;
  for( auto p : cameras->cameras() )
  {
    auto c =
      std::dynamic_pointer_cast< simple_camera_perspective >( p.second );
    if( c )
    {
      cams.insert( p.first, c );
    }
  }

  auto lms = landmarks->landmarks();
  this->optimize( cams, lms, tracks, {}, {}, constraints );
  landmarks = std::make_shared< simple_landmark_map >( lms );
  cameras = std::make_shared< simple_camera_perspective_map >( cams );
}

// ----------------------------------------------------------------------------
// Optimize the camera and landmark parameters given a set of tracks
void
bundle_adjust
::optimize(
  kwiver::vital::simple_camera_perspective_map& cameras,
  kwiver::vital::landmark_map::map_landmark_t& landmarks,
  vital::feature_track_set_sptr tracks,
  const std::set< vital::frame_id_t >& to_fix_cameras_in,
  const std::set< vital::landmark_id_t >& to_fix_landmarks_in,
  kwiver::vital::sfm_constraints_sptr constraints ) const
{
  if( !tracks )
  {
    // TODO throw an exception for missing input data
    return;
  }

  std::unordered_set< vital::frame_id_t > to_fix_cameras;
  for( auto& fid : to_fix_cameras_in )
  {
    to_fix_cameras.insert( fid );
  }

  std::unordered_set< vital::landmark_id_t > to_fix_landmarks;
  for( auto& lid : to_fix_landmarks_in )
  {
    to_fix_landmarks.insert( lid );
  }

  std::set< frame_id_t > fixed_cameras;

  // extract data from containers
  d_->cams = cameras.cameras();
  d_->lms = landmarks;

  // Extract the landmark locations into a mutable map
  d_->landmark_params.clear();
  for( const landmark_map::map_landmark_t::value_type& lm : d_->lms )
  {
    landmark_id_t lm_id = lm.first;

    if( d_->landmark_params.find( lm_id ) == d_->landmark_params.end() )
    {
      vector_3d loc = lm.second->loc();
      d_->landmark_params[ lm_id ] =
        std::vector< double >( loc.data(), loc.data() + 3 );
    }
  }

  using lm_param_map_t =
    std::unordered_map< track_id_t, std::vector< double > >;

  d_->camera_params.clear();
  d_->camera_intr_params.clear();
  d_->frame_to_intr_map.clear();

  // Extract the raw camera parameter into the provided maps
  c_camera_options->extract_camera_parameters(
    d_->cams,
    d_->camera_params,
    d_->camera_intr_params,
    d_->frame_to_intr_map );

  // the Ceres solver problem
  ::ceres::Problem problem;

  // enumerate the intrinsics held constant
  std::vector< int > constant_intrinsics =
    c_camera_options->enumerate_constant_intrinsics();

  // Create the loss function to use
  ::ceres::LossFunction* loss_func =
    LossFunctionFactory(
      c_loss_function_type,
      c_loss_function_scale );
  bool loss_func_used = false;

  // Add the residuals for each relevant observation
  std::set< unsigned int > used_intrinsics;

  for( const auto& lm : d_->lms )
  {
    const auto lm_id = lm.first;
    bool lm_visible_in_variable_camera = false;
    // lowest index track is landmark id

    auto t = tracks->get_track( lm_id );
    if( !t )
    {
      continue;
    }

    auto lm_itr = d_->landmark_params.find( lm_id );
    // skip this track if the landmark is not in the set to optimize
    if( lm_itr == d_->landmark_params.end() )
    {
      continue;
    }
    for( auto ts : *t )
    {
      if( to_fix_cameras.find( ts->frame() ) == to_fix_cameras.end() )
      {
        // this landmark is viewed in a variable camera.  So include it in the
        // state to estimate.
        lm_visible_in_variable_camera = true;
        break;
      }
    }

    if( !lm_visible_in_variable_camera )
    {
      // this landmark is not visible in a variable camera, so no need to add
      // measurements for it.
      continue;
    }

    bool fixed_landmark = to_fix_landmarks.find( lm_id ) !=
                          to_fix_landmarks.end();

    for( auto ts : *t )
    {
      auto cam_itr = d_->camera_params.find( ts->frame() );
      if( cam_itr == d_->camera_params.end() )
      {
        continue;
      }

      bool fixed_camera = to_fix_cameras.find( cam_itr->first ) !=
                          to_fix_cameras.end();

      if( fixed_landmark && fixed_camera )
      {
        // skip this measurement because it involves both a fixed camera and
        // fixed landmark.
        continue;
      }

      auto fts = std::dynamic_pointer_cast< feature_track_state >( ts );
      if( !fts || !fts->feature )
      {
        continue;
      }
      if( !fts->inlier )
      {
        continue; // feature is not an inlier so don't use it in ba.
      }

      unsigned intr_idx = d_->frame_to_intr_map[ fts->frame() ];
      double* intr_params_ptr = &d_->camera_intr_params[ intr_idx ][ 0 ];
      used_intrinsics.insert( intr_idx );

      vector_2d pt = fts->feature->loc();
      problem.AddResidualBlock(
        create_cost_func(
          c_camera_options->lens_distortion_type,
          pt.x(), pt.y() ),
        loss_func,
        intr_params_ptr,
        &cam_itr->second[ 0 ],
        &lm_itr->second[ 0 ] );

      loss_func_used = true;
    }
  }

  if( c_camera_options->camera_path_smoothness > 0.0 ||
      c_camera_options->camera_forward_motion_damping > 0.0 )
  {
    // sort the camera parameters in order of frame number
    std::vector< std::pair< vital::frame_id_t, double* > > ordered_params;
    for( auto& item : d_->camera_params )
    {
      ordered_params.push_back(
        std::make_pair(
          item.first,
          &item.second[ 0 ] ) );
    }
    std::sort( ordered_params.begin(), ordered_params.end() );

    // Add camera path regularization residuals
    c_camera_options->add_camera_path_smoothness_cost(
      problem,
      ordered_params );

    // Add forward motion regularization residuals
    c_camera_options->add_forward_motion_damping_cost(
      problem, ordered_params,
      d_->frame_to_intr_map );
  }

  // fix all the cameras in the to_fix_cameras list
  std::unordered_set< unsigned int > to_fix_intrinsics;
  for( auto tfc : to_fix_cameras )
  {
    auto cam_itr = d_->camera_params.find( tfc );
    if( cam_itr == d_->camera_params.end() )
    {
      continue;
    }

    double* state_ptr = &cam_itr->second[ 0 ];
    if( problem.HasParameterBlock( state_ptr ) )
    {
      problem.SetParameterBlockConstant( state_ptr );
      fixed_cameras.insert( tfc );
    }

    // Mark the intrinsics for this camera fixed as well.
    // Only optimize intrinsics if no cameras using these
    // intrinsics are fixed
    auto const& intr_itr = d_->frame_to_intr_map.find( tfc );
    if( intr_itr != d_->frame_to_intr_map.end() )
    {
      to_fix_intrinsics.insert( intr_itr->second );
    }
  }

  std::set< landmark_id_t > fixed_landmarks;
  // fix all the landmarks in the to_fix_landmarks list
  for( auto tfl : to_fix_landmarks )
  {
    auto lm_id = tfl;

    auto lm_itr = d_->landmark_params.find( lm_id );
    if( lm_itr == d_->landmark_params.end() )
    {
      continue;
    }

    double* state_ptr = &lm_itr->second[ 0 ];
    if( problem.HasParameterBlock( state_ptr ) )
    {
      problem.SetParameterBlockConstant( state_ptr );
      fixed_landmarks.insert( tfl );
    }
  }

  // add costs for priors
  int num_position_priors_applied =
    c_camera_options->add_position_prior_cost(
      problem, d_->camera_params,
      constraints );

  c_camera_options->add_intrinsic_priors_cost(
    problem,
    d_->camera_intr_params );

  if( num_position_priors_applied < 3 )
  {
    // gauge fixing code
    if( fixed_cameras.size() == 0 && fixed_landmarks.size() < 3 )
    {
      // If no cameras are fixed, find the first camera and fix it.
      for( auto& fix : d_->camera_params )
      {
        auto fixed_fid = fix.first;
        auto state = &fix.second[ 0 ];
        if( problem.HasParameterBlock( state ) )
        {
          problem.SetParameterBlockConstant( state );
          fixed_cameras.insert( fixed_fid );
          break;
        }
      }
    }

    if( fixed_cameras.size() == 1 && fixed_landmarks.empty() )
    {
      // add measurement between the one fixed camera and another arbitrary
      // camera to fix the scale
      auto cam_itr_0 = d_->camera_params.find( *fixed_cameras.begin() );
      // get another arbitrary camera
      bool scale_locking_camera_found = false;
      auto cam_itr_1 = d_->camera_params.begin();
      for(; cam_itr_1 != d_->camera_params.end(); ++cam_itr_1 )
      {
        if( cam_itr_1->first != cam_itr_0->first &&
            problem.HasParameterBlock( &cam_itr_1->second[ 0 ] ) )
        {
          scale_locking_camera_found = true;
          break;
        }
      }

      if( scale_locking_camera_found )
      {
        double* param0 = &cam_itr_0->second[ 0 ];
        double* param1 = &cam_itr_1->second[ 0 ];
        double distance_squared =
          ( Eigen::Map< vector_3d >( param0 + 3 ) -
            Eigen::Map< vector_3d >( param1 + 3 ) ).squaredNorm();
        double scale = problem.NumResiduals() / distance_squared;

        auto dist_loss =
          new ::ceres::ScaledLoss(
            NULL, scale,
            ::ceres::Ownership::TAKE_OWNERSHIP );
        problem.AddResidualBlock(
          distance_constraint::create(
            distance_squared ),
          dist_loss, param0, param1 );
      }
    }
  }

  const unsigned int ndp =
    num_distortion_params( c_camera_options->lens_distortion_type );
  for( const unsigned int idx : used_intrinsics )
  {
    std::vector< double >& cip = d_->camera_intr_params[ idx ];
    // apply the constraints
    if( constant_intrinsics.size() > 4 + ndp ||
        to_fix_intrinsics.count( idx ) > 0 )
    {
      // set all parameters in the block constant
      problem.SetParameterBlockConstant( &cip[ 0 ] );
    }
    else if( !constant_intrinsics.empty() )
    {
      // set a subset of parameters in the block constant
      problem.SetParameterization(
        &cip[ 0 ],
        new ::ceres::SubsetParameterization(
          5 +
          ndp,
          constant_intrinsics ) );
    }
  }

  // If the loss function was added to a residual block, ownership was
  // transfered.  If not then we need to delete it.
  if( loss_func && !loss_func_used )
  {
    delete loss_func;
  }

  ::ceres::Solver::Summary summary;
  ::ceres::Solve( c_solver_options->options, &problem, &summary );
  if( c_log_full_report )
  {
    LOG_DEBUG( logger(), "Ceres Full Report:\n" << summary.FullReport() );
  }

  // Update the landmarks with the optimized values
  for( const lm_param_map_t::value_type& lmp : d_->landmark_params )
  {
    auto lmi = std::static_pointer_cast< landmark_d >( d_->lms[ lmp.first ] );
    lmi->set_loc( Eigen::Map< const vector_3d >( &lmp.second[ 0 ] ) );
  }

  // Update the cameras with the optimized values
  c_camera_options->update_camera_parameters(
    d_->cams, d_->camera_params,
    d_->camera_intr_params,
    d_->frame_to_intr_map );
  cameras.set_from_base_camera_map( d_->cams );
}

// ----------------------------------------------------------------------------
// Set a callback function to report intermediate progress
void
bundle_adjust
::set_callback( callback_t cb )
{
  kwiver::vital::algo::bundle_adjust::set_callback( cb );
}

// ----------------------------------------------------------------------------
// This function is called by a Ceres callback to trigger a kwiver callback
bool
bundle_adjust
::trigger_callback()
{
  if( this->m_callback )
  {
    if( !c_solver_options->options.update_state_every_iteration )
    {
      return this->m_callback( nullptr, nullptr, nullptr );
    }
    // Update the landmarks with the optimized values
    typedef std::map< track_id_t, std::vector< double > > lm_param_map_t;
    for( const lm_param_map_t::value_type& lmp : d_->landmark_params )
    {
      auto lmi =
        std::static_pointer_cast< landmark_d >( d_->lms[ lmp.first ] );
      lmi->set_loc( Eigen::Map< const vector_3d >( &lmp.second[ 0 ] ) );
    }

    landmark_map_sptr landmarks = std::make_shared< simple_landmark_map >(
      d_->lms );

    // Update the cameras with the optimized values
    c_camera_options->update_camera_parameters(
      d_->cams, d_->camera_params,
      d_->camera_intr_params,
      d_->frame_to_intr_map );

    camera_map_sptr cameras =
      std::make_shared< simple_camera_map >( d_->cams );

    return this->m_callback( cameras, landmarks, nullptr );
  }
  return true;
}

} // namespace ceres

} // namespace arrows

} // namespace kwiver
