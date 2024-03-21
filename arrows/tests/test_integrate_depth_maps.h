// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief test VXL image class functionality

#include <arrows/core/mesh_operations.h>
#include <arrows/core/render_mesh_depth_map.h>

#include <vital/types/camera_perspective_map.h>
#include <vital/util/cpu_timer.h>
#include <vital/util/transform_image.h>

#include <gtest/gtest.h>

namespace {

void
make_test_data(
  std::vector< kwiver::vital::image_container_sptr >& depth_maps,
  std::vector< kwiver::vital::camera_perspective_sptr >& cameras,
  kwiver::vital::vector_3d& min_pt, kwiver::vital::vector_3d& max_pt,
  kwiver::vital::simple_camera_intrinsics const& K )
{
  namespace core = kwiver::arrows::core;

  // create two stacked boxes on a ground plane
  auto cube = kwiver::testing::cube_mesh( 1.0 );
  cube->merge( *kwiver::testing::cube_mesh( 0.5, { 0.0, 0.0, 0.75 } ) );
  cube->merge(
    *kwiver::testing::grid_mesh(
      20, 20, 1.0,
      { -10.0, -10.0, -0.5 } ) );

  min_pt = kwiver::vital::vector_3d{ -1.0, -1.0, -0.7 };
  max_pt = kwiver::vital::vector_3d{ 1.0, 1.0, 1.2 };

  // convert to triangles for rendering
  core::mesh_triangulate( *cube );

  // create a camera sequence (elliptical path)
  auto cams = kwiver::testing::camera_seq( 10, K, 1.0, 360.0 );
  kwiver::vital::camera_perspective_map pcameras;
  pcameras.set_from_base_cams( cams );

  depth_maps.clear();
  cameras.clear();
  for( auto const& camera : pcameras.T_cameras() )
  {
    depth_maps.push_back( core::render_mesh_depth_map( cube, camera.second ) );
    cameras.push_back( camera.second );
  }
}

void
evaluate_volume(
  kwiver::vital::image_container_sptr volume,
  kwiver::vital::vector_3d const& min_pt,
  kwiver::vital::vector_3d const& max_pt,
  kwiver::vital::vector_3d const& spacing )
{
  kwiver::vital::vector_3d sizes = max_pt - min_pt;
  EXPECT_NEAR( spacing[ 0 ] * volume->width(), sizes[ 0 ], spacing[ 0 ] );
  EXPECT_NEAR( spacing[ 1 ] * volume->height(), sizes[ 1 ], spacing[ 1 ] );
  EXPECT_NEAR( spacing[ 2 ] * volume->depth(), sizes[ 2 ], spacing[ 2 ] );

  // helper function to look up volume values in global coordinates
  kwiver::vital::image_of< double > vol_data( volume->get_image() );
  auto world_value = [ &vol_data, spacing,
                       min_pt ](kwiver::vital::vector_3d const& v){
                       Eigen::Vector3i index = ( ( v - min_pt ).array() /
                                                 spacing.array() ).cast< int >();
                       if( index[ 0 ] < 0 ||
                           index[ 0 ] >=
                           static_cast< int >( vol_data.width() ) ||
                           index[ 1 ] < 0 ||
                           index[ 1 ] >=
                           static_cast< int >( vol_data.height() ) ||
                           index[ 2 ] < 0 ||
                           index[ 2 ] >=
                           static_cast< int >( vol_data.depth() ) )
                       {
                         return std::numeric_limits< double >::quiet_NaN();
                       }
                       return vol_data( index[ 0 ], index[ 1 ], index[ 2 ] );
                     };

  // values inside the structure should have positive values
  EXPECT_GT( world_value( { 0.0, 0.0, 0.0 } ), 0.0 );
  EXPECT_GT( world_value( { 0.0, 0.0, -0.6 } ), 0.0 );
  EXPECT_GT( world_value( { 0.0, 0.0, 0.9 } ), 0.0 );
  EXPECT_GT( world_value( { -0.75, -0.75, -0.6 } ), 0.0 );

  // values near the boundary should have small values
  EXPECT_NEAR( world_value( { 0.5, 0.0, 0.0 } ), 0.0, 1.0 );
  EXPECT_NEAR( world_value( { 0.0, 0.5, 0.0 } ), 0.0, 1.0 );
  EXPECT_NEAR( world_value( { 0.49, 0.49, 0.0 } ), 0.0, 1.0 );
  EXPECT_NEAR( world_value( { 0.0, 0.0, 1.0 } ), 0.0, 1.0 );

  // values inside the structure should have positive values
  EXPECT_LT( world_value( { 0.0, 0.0, 1.1 } ), 0.0 );
  EXPECT_LT( world_value( { 0.5, 0.5, 0.6 } ), 0.0 );
  EXPECT_LT( world_value( { -0.75, -0.75, -0.4 } ), 0.0 );
}

} // end anonymous namespace
