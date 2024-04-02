// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <arrows/core/algo/create_detection_grid.h>
#include <arrows/core/algo/filter_features_scale.h>
#include <vital/plugin_management/pluggable_macro_testing.h>
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
TEST ( create_detection_grid, create )
{
  plugin_manager::instance().load_all_plugins();

  EXPECT_NE(
    nullptr,
    create_algorithm< algo::image_object_detector >(
      "create_detection_grid" ) );
}

// ----------------------------------------------------------------------------
TEST ( create_detection_grid, default_config )
{
  EXPECT_PLUGGABLE_IMPL(
    create_detection_grid,
    "Create a grid of detections across the input image.",
    PARAM_DEFAULT(
      width, double,
      "Width of each detection in the output grid.",
      0.0 ),
    PARAM_DEFAULT(
      height, double,
      "Height of each detection in the output grid.",
      0.0 ),
    PARAM_DEFAULT(
      x_step, double,
      "How far apart along the x axis each detection is.",
      0.0 ),
    PARAM_DEFAULT(
      y_step, double,
      "How far apart along the y axis each detection is.",
      0.0 )
  );
}
