// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/**
 * \file
 * \brief Implementation of hierarchical_bundle_adjust
 */

#include "hierarchical_bundle_adjust.h"

#include <algorithm>
#include <iostream>
#include <limits>
#include <map>
#include <vector>

#include <math.h>

#include <vital/util/cpu_timer.h>

#include <arrows/mvg/interpolate_camera.h>
#include <arrows/mvg/metrics.h>
#include <vital/algo/optimize_cameras.h>
#include <vital/algo/triangulate_landmarks.h>

#include <vital/exceptions.h>
#include <vital/types/camera_perspective.h>
#include <vital/vital_types.h>

using namespace kwiver::vital;

namespace kwiver {

namespace arrows {

namespace mvg {

namespace // anonymous
{

// subsample a every Nth camera

/**
 * Subsamples are chosen based on camera order index instead of frame nubmer,
 * as cameras given may not be in sequential order.
 *
 * The first camera in the map is given index 0 and the last given index
 * (cameras.size() - 1).
 */
camera_map::map_camera_t
subsample_cameras( camera_map::map_camera_t const& cameras, unsigned n )
{
  kwiver::vital::scoped_cpu_timer t( "Camera sub-sampling" );

  // if sub-sample is 1, no sub-sampling occurs, just return a copy of the map
  if( n == 1 )
  {
    return cameras;
  }

  camera_map::map_camera_t subsample;
  unsigned int i = 0;
  for( camera_map::map_camera_t::value_type const& p : cameras )
  {
    if( i % n == 0 )
    {
      subsample[ p.first ] = p.second;
    }
    ++i;
  }
  return subsample;
}

// ----------------------------------------------------------------------------
// Integer interpolation -- used with indices, so can assume positive
frame_id_t
int_interp( frame_id_t a, frame_id_t b, double p )
{
  // intend for static cast acts as floor in rounding.
  return static_cast< frame_id_t >( a * ( 1.0 - p ) + b * p + 0.5 );
}

} // end anonymous namespace

// ============================================================================
// private implementation / data container for hierarchical_bundle_adjust
class hierarchical_bundle_adjust::priv
{
public:
  priv( hierarchical_bundle_adjust& parent )
    : parent( parent )
  {}

  hierarchical_bundle_adjust& parent;
  // Configuration values
  unsigned int c_initial_sub_sample() { return parent.c_initial_sub_sample; }
  unsigned int c_interpolation_rate() { return parent.c_interpolation_rate; }
  bool c_rmse_reporting_enabled() { return parent.c_rmse_reporting_enabled; }

  // processing classes
  vital::algo::bundle_adjust_sptr d_sba_impl() { return parent.c_sba_impl; }

  vital::algo::optimize_cameras_sptr
  d_camera_optimizer()
  {
    return parent.c_camera_optimizer;
  }

  vital::algo::triangulate_landmarks_sptr
  d_lm_triangulator()
  {
    return parent.c_lm_triangulator;
  }
};

// ----------------------------------------------------------------------------
// Constructor
void
hierarchical_bundle_adjust
::initialize()
{
  KWIVER_INITIALIZE_UNIQUE_PTR( priv, d_ );
  attach_logger( "arrows.mvg.hierarchical_bundle_adjust" );
}

// Destructor
hierarchical_bundle_adjust
::~hierarchical_bundle_adjust() noexcept
{}

// ----------------------------------------------------------------------------
// Check that the algorithm's configuration vital::config_block is valid
bool
hierarchical_bundle_adjust
::check_configuration( vital::config_block_sptr config ) const
{
  bool valid = true;

#define HSBA_CHECK_FAIL( msg )                     \
LOG_DEBUG(logger(), "Config Check Fail: " << msg); \
valid = false

  // using long to allow negatives and maintain numerical capacity of
  // unsigned int as the values would otherwise be stored as.
  if( config->has_value( "initial_sub_sample" ) &&
      config->get_value< long >( "initial_sub_sample" ) <= 0 )
  {
    HSBA_CHECK_FAIL(
      "\"initial_sub_sample\" must be greater than 0. Given: "
        << config->get_value< long >( "initial_sub_sample" ) );
  }
  if( config->has_value( "interpolation_rate" ) &&
      config->get_value< long >( "interpolation_rate" ) < 0 )
  {
    HSBA_CHECK_FAIL(
      "\"interpolation_rate\" must be >= 0. Given: "
        << config->get_value< long >( "interpolation_rate" ) );
  }

  if( !vital::check_nested_algo_configuration< vital::algo::bundle_adjust >(
    "sba_impl", config ) )
  {
    HSBA_CHECK_FAIL( "sba_impl configuration invalid." );
  }

  if( config->get_value< std::string >( "camera_optimizer:type", "" ) == "" )
  {
    LOG_DEBUG(logger(), "HSBA per-iteration camera optimization disabled" );
  }
  else if( !vital::check_nested_algo_configuration< vital::algo::optimize_cameras >( "camera_optimizer", config ) )
  {
    HSBA_CHECK_FAIL( "camera_optimizer configuration invalid." );
  }

  if( config->get_value< std::string >( "lm_triangulator:type", "" ) == "" )
  {
    LOG_DEBUG(logger(), "HSBA per-iteration LM Triangulation disabled" );
  }
  else if( !vital::check_nested_algo_configuration< vital::algo::triangulate_landmarks >( "lm_triangulator", config ) )
  {
    LOG_DEBUG(
      logger(), "lm_triangulator type: \""
        << config->get_value< std::string >( "lm_triangulator:type" ) << "\"" );
    HSBA_CHECK_FAIL( "lm_triangulator configuration invalid." );
  }

#undef HSBA_CHECK_FAIL

  // camera optimizer and lm triangulator are optional. If not set, pointers
  // will be 0.

  return valid;
}

// ----------------------------------------------------------------------------

// Optimize the camera and landmark parameters given a set of feature tracks

/**
 * Making naive assuptions:
 *  - cameras we are given are in sequence (no previous sub-sampling and no
 * frame gaps)
 *  - given camera map evenly interpolates with the current configuration
 *  - Assuming that all frames we interpolate have tracks/landmarks with which
 *    to optimize that camera over.
 */
void
hierarchical_bundle_adjust
::optimize(
  camera_map_sptr& cameras,
  landmark_map_sptr& landmarks,
  feature_track_set_sptr tracks,
  sfm_constraints_sptr constraints ) const
{
  using namespace std;

  // frame_id_t orig_max_frame = cameras->cameras().rbegin()->first;
  LOG_INFO(logger(), cameras->size() << " cameras provided" );

  size_t num_orig_cams = tracks->all_frame_ids().size();

  // If interpolation rate is 0, then that means that all intermediate frames
  // should be interpolated on the first step. Due to how the algorithm
  // functions, set var to unsigned int max.
  frame_id_t ir = d_->c_interpolation_rate();
  if( ir == 0 )
  {
    ir = std::numeric_limits< frame_id_t >::max();
  }
  LOG_DEBUG(logger(), "Interpolation rate: " << ir);

  // Sub-sample cameras
  // Always adding the last camera (if not already in there) to the sub-
  // sampling in order to remove the complexity of interpolating into empty
  // space (constant operation).
  unsigned int ssr = d_->c_initial_sub_sample();
  camera_map::map_camera_t input_cams = cameras->cameras(),
    acm;
  acm = subsample_cameras( input_cams, ssr );
  acm[ input_cams.rbegin()->first ] = input_cams.rbegin()->second;

  camera_map_sptr active_cam_map( new simple_camera_map( acm ) );
  LOG_INFO( logger(), "Subsampled cameras: " << active_cam_map->size() );

  // need to have at least 2 cameras
  if( active_cam_map->size() < 2 )
  {
    VITAL_THROW( invalid_value, "Camera map given is of insufficient length." );
  }

  bool done = false;
  do
  {
    LOG_INFO(
      logger(),
      "Optimizing " << active_cam_map->size() << " active cameras" );
    // updated active_cam_map and landmarks
    {
      // scope block
      kwiver::vital::scoped_cpu_timer t( "inner-SBA iteration" );
      d_->d_sba_impl()->optimize(
        active_cam_map, landmarks, tracks,
        constraints );
    }

    double rmse = reprojection_rmse(
      active_cam_map->cameras(),
      landmarks->landmarks(),
      tracks->tracks() );
    LOG_DEBUG(logger(), "current RMSE: " << rmse);

    // If we've just completed SBA with all original frames in the new map,
    // then we're done.
    LOG_DEBUG(
      logger(), "completion check: " << active_cam_map->size()
                                     << " >= " << num_orig_cams );
    if( active_cam_map->size() >= num_orig_cams )
    {
      LOG_INFO(logger(), "complete" );
      done = true;
    }
    // perform interpolation between frames that have gaps in between them
    else
    {
      camera_map::map_camera_t
      // separated interpolated camera storage
        interped_cams,
      // concrete map of current active cameras
        ac_map = active_cam_map->cameras();

      // pre-allocation of variables for performance
      size_t ir_l; // local interpolation rate as gaps available may be less
                   // than global rate
      double f;
      frame_id_t i2;
      frame_id_t cur_frm, next_frm;
      camera_perspective_sptr cur_cam, next_cam;

      // Iterate through frames and cameras, interpolating across gaps when
      // found
      // ASSUMING even interpolation for now
      camera_map::map_camera_t::const_iterator it = ac_map.begin();
      {
        // scope block
        kwiver::vital::scoped_cpu_timer t( "interpolating cams" );
        while( it != ac_map.end() )
        {
          cur_frm = it->first;
          cur_cam =
            std::dynamic_pointer_cast< camera_perspective >( it->second );
          ++it;

          // If we're not at the end of the active camera sequence
          if( it != ac_map.end() )
          {
            next_frm = it->first;
            next_cam =
              std::dynamic_pointer_cast< camera_perspective >( it->second );

            // this specific gap's interpolation rate -- gap may be smaller than
            // ir
            ir_l = std::min( ir, next_frm - cur_frm - 1 );

            for( double i = 1; i <= ir_l; ++i )
            {
              // Determine the integer associated with the interpolation step,
              // then determine the fraction location of that integer between
              // the two end points.

              // absolute fraction, might not land on integer
              f = i / ( ir_l + 1 );

              // aproximate interpolation snapped to nearest integer
              i2 = int_interp( cur_frm, next_frm, f );

              // fraction position of interpoated integer
              f = static_cast< double >( i2 - cur_frm ) /
                  ( next_frm - cur_frm );

              interped_cams[ i2 ] = interpolate_camera( cur_cam, next_cam, f );
            }
          }
        }
      }
      if( interped_cams.empty() )
      {
        LOG_INFO(logger(), "No new cameras interpolated, done." );
        break;
      }

      camera_map_sptr interped_cams_p( new simple_camera_map( interped_cams ) );

      // Optimize new camers
      if( d_->d_camera_optimizer() )
      {
        LOG_INFO(
          logger(), "Optimizing new interpolated cameras ("
            << interped_cams.size() << " cams)" );
        if( d_->c_rmse_reporting_enabled() )
        {
          LOG_DEBUG(
            logger(), "pre-optimization RMSE : "
              << reprojection_rmse(
              interped_cams_p->cameras(),
              landmarks->landmarks(),
              tracks->tracks() ) );
        }

        {
          // scope block
          kwiver::vital::scoped_cpu_timer t( "\t- cameras optimization" );
          d_->d_camera_optimizer()->optimize(
            interped_cams_p, tracks,
            landmarks, constraints );
        }

        if( d_->c_rmse_reporting_enabled() )
        {
          LOG_DEBUG(
            logger(), "post-optimization RMSE : "
              << reprojection_rmse(
              interped_cams_p->cameras(),
              landmarks->landmarks(),
              tracks->tracks() ) );
        }
      }
      // adding optimized interpolated cameras to the map of existing cameras
      for( camera_map::map_camera_t::value_type const& p :
           interped_cams_p->cameras() )
      {
        ac_map[ p.first ] = p.second;
      }
      // Create new sptr of modified ac_map
      active_cam_map = camera_map_sptr( new simple_camera_map( ac_map ) );
      if( d_->c_rmse_reporting_enabled() )
      {
        LOG_DEBUG(
          logger(), "combined map RMSE : "
            << reprojection_rmse(
            active_cam_map->cameras(),
            landmarks->landmarks(),
            tracks->tracks() ) );
      }

      // LM triangulation
      if( d_->d_lm_triangulator() )
      {
        LOG_INFO(
          logger(),
          "Triangulating landmarks after interpolating cameras" );
        if( d_->c_rmse_reporting_enabled() )
        {
          LOG_DEBUG(
            logger(), "pre-triangulation RMSE : "
              << reprojection_rmse(
              active_cam_map->cameras(),
              landmarks->landmarks(),
              tracks->tracks() ) );
        }

        {
          // scoped block
          kwiver::vital::scoped_cpu_timer t( "\t- lm triangulation" );
          d_->d_lm_triangulator()->triangulate(
            active_cam_map, tracks,
            landmarks );
        }

        if( d_->c_rmse_reporting_enabled() )
        {
          LOG_DEBUG(
            logger(), "post-triangulation RMSE : "
              << reprojection_rmse(
              active_cam_map->cameras(),
              landmarks->landmarks(),
              tracks->tracks() ) );
        }
      }
    }
  } while( !done );

  // push up the resultant cameras
  cameras = active_cam_map;
}

} // end namespace mvg

} // end namespace arrows

} // end namespace kwiver
