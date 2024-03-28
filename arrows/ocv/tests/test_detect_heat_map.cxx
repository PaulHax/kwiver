// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <arrows/ocv/algo/detect_heat_map.h>

#include <vital/plugin_management/pluggable_macro_testing.h>
#include <vital/plugin_management/plugin_manager.h>

#include <vital/algo/algorithm.txx>

#include <gtest/gtest.h>

using namespace kwiver::vital;
using namespace kwiver::arrows;

// ----------------------------------------------------------------------------
int
main( int argc, char** argv )
{
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}

// ----------------------------------------------------------------------------
TEST ( detect_heat_map, create )
{
  plugin_manager::instance().load_all_plugins();
  EXPECT_NE(
    nullptr, create_algorithm< algo::image_object_detector >(
      "detect_heat_map" ) );
}

// ----------------------------------------------------------------------------
TEST ( detect_heat_map, default_config )
{
  using namespace kwiver::arrows::ocv;

  EXPECT_PLUGGABLE_IMPL(
    detect_heat_map,
    "OCV implementation to create detections from heatmaps",

    PARAM_DEFAULT(
      threshold, double,
      "Threshold value applied to each pixel of the heat map to "
      "turn it into a binary mask. Any pixels with value "
      "strictly greater than this threshold will be turned on "
      "in the mask. Detection objects will be associated with "
      "connected-component regions of above-threshold pixels. "
      "The default threshold of -1 indicates that further "
      "processing will be done on the full-range heat map "
      "image. This mode of processing requires that ",
      -1 ),

    PARAM_DEFAULT(
      force_bbox_width, int,
      "Create bounding boxes of this fixed width.",
      -1 ),

    PARAM_DEFAULT(
      force_bbox_height, int,
      "Create bounding boxes of this fixed height.",
      -1 ),

    PARAM_DEFAULT(
      score_mode, std::string,
      "Mode in which a score is attributed to each detected"
      "object. A numerical value indicates that all detected"
      "objects will be assigned this fixed score. No other"
      "modes are defined at this time.",
      "1" ),

    PARAM_DEFAULT(
      bbox_buffer, int,
      "If a bounding box of fixed height and width is specified,"
      "the default bbox_buffer of 0 indicates that the bounding"
      "boxes will tightly crop features in the heat map, and "
      "multiple, non-overlapping bounding boxes will be created "
      "to cover large, extended heat-map features. With a value "
      "greater than 0, generated bounding boxes will tend to "
      "have that number of pixels of buffer from the heat-map "
      "features. Also, setting bbox_buffer causes the generated "
      "bounding boxes to tend to overlap by this number of "
      "pixels when multiple boxes are required to cover and "
      "extended heat-map feature.", 0 ),

    PARAM_DEFAULT(
      min_area, int,
      "Minimum area of above-threshold pixels in a connected "
      "cluster allowed. Area is approximately equal to the "
      "number of pixels in the cluster.",
      1 ),

    PARAM_DEFAULT(
      max_area, int,
      "Maximum area of above-threshold pixels in a connected "
      "cluster allowed. Area is approximately equal to the "
      "number of pixels in the cluster.",
      10000000 ),

    PARAM_DEFAULT(
      min_fill_fraction, double,
      "Fraction of the bounding box filled with above threshold "
      "pixels.",
      0.25 ),

    PARAM_DEFAULT(
      class_name, std::string,
      "Detection class name.",
      "unspecified" ),

    PARAM_DEFAULT(
      max_boxes, int,
      "Maximum number of "
      "bounding boxes to generate. If exceeded, the top "
      "'max_boxes' ones will be returned", 1000000 ),

    PARAM_DEFAULT(
      pyr_red_levels, int,
      "Levels of image "
      "pyramid reduction (decimation) on the heat map before "
      "box selection. This improves speed at the expense of "
      "coarseness of bounding box placement. ", 0 )
  );
}
