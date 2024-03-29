// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <arrows/core/algo/example_detector.h>
#include <vital/plugin_management/plugin_manager.h>

#include <gtest/gtest.h>

#include <algorithm>

using namespace kwiver::vital;
using namespace kwiver::arrows::core;

// ----------------------------------------------------------------------------
int
main( int argc, char** argv )
{
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}

// ----------------------------------------------------------------------------
TEST ( example_detector, create )
{
  plugin_manager::instance().load_all_plugins();

  EXPECT_NE(
    nullptr,
    create_algorithm< algo::image_object_detector >( "example_detector" ) );
}

// ----------------------------------------------------------------------------
TEST ( example_detector, default_config )
{
  EXPECT_PLUGGABLE_IMPL(
    example_detector,
    "Simple example detector that just creates a user-specified bounding box.",
    PARAM_DEFAULT(
      center_x, double,
      "Bounding box center x coordinate.",
      100.0 ),
    PARAM_DEFAULT(
      center_y, double,
      "Bounding box center y coordinate.",
      100.0 ),
    PARAM_DEFAULT(
      height, double,
      "Bounding box height.",
      200.0 ),
    PARAM_DEFAULT(
      width, double,
      "Bounding box width.",
      200.0 ),
    PARAM_DEFAULT(
      dx, double,
      "Bounding box x translation per frame.",
      0.0 ),
    PARAM_DEFAULT(
      dy, double,
      "Bounding box y translation per frame.",
      0.0 )
  );
}
