// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <arrows/ocv/algo/detect_motion_3frame_differencing.h>
#include <arrows/ocv/algo/detect_motion_mog2.h>
#include <vital/plugin_management/pluggable_macro_testing.h>
#include <vital/plugin_management/plugin_manager.h>

#include <vital/algo/algorithm.txx>

#include <gtest/gtest.h>

using namespace kwiver::vital;
using namespace kwiver::arrows::ocv;

// ----------------------------------------------------------------------------
int
main( int argc, char** argv )
{
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}

// ----------------------------------------------------------------------------
TEST ( detect_motion_mog2, create )
{
  plugin_manager::instance().load_all_plugins();
  EXPECT_NE(
    nullptr, create_algorithm< algo::detect_motion >( "ocv_mog2" ) );
}

// ----------------------------------------------------------------------------
TEST ( detect_motion_3frame_differencing, create )
{
  plugin_manager::instance().load_all_plugins();
  EXPECT_NE(
    nullptr, create_algorithm< algo::detect_motion >(
      "ocv_3frame_differencing" ) );
}

// ----------------------------------------------------------------------------
TEST ( detect_motion_mog2, default_config )
{
  EXPECT_PLUGGABLE_IMPL(
    detect_motion_mog2,
    "OCV implementation of detect_motion using cv::BackgroundSubtractormog2",

    PARAM_DEFAULT(
      var_threshold,
      double,
      "Threshold on the squared Mahalanobis distance between "
      "the pixel and the model to decide whether a pixel is "
      "well described by the background model. This parameter "
      "does not affect the background update.",
      36 ),

    PARAM_DEFAULT(
      history,
      int,
      "Length of the history.",
      100 ),

    PARAM_DEFAULT(
      learning_rate,
      double,
      "determines how quickly features are “forgotten” from "
      "histograms (range 0-1).",
      0.01 ),

    PARAM_DEFAULT(
      blur_kernel_size,
      int,
      "Diameter of the normalized box filter blurring "
      "kernel (positive integer).",
      3 ),

    PARAM_DEFAULT(
      min_frames,
      int,
      "Minimum frames that need to be included in the "
      "background model before detections are emmited.",
      1 ),

    PARAM_DEFAULT(
      max_foreground_fract,
      double,
      "Specifies the maximum expected fraction of the scene "
      "that may contain foreground movers at any time. When the "
      "fraction of pixels determined to be in motion exceeds "
      "this value, the background model is assumed to be "
      "invalid (e.g., due to excessive camera motion) and is "
      "reset. The default value of 1 indicates that no checking "
      "is done.",
      1 )
  );
}

// ----------------------------------------------------------------------------
TEST ( detect_motion_3frame_differencing, default_config )
{
  EXPECT_PLUGGABLE_IMPL(
    detect_motion_3frame_differencing,
    "OCV implementation of detect_motion using three-frame differencing",

    PARAM_DEFAULT(
      frame_separation,
      std::size_t,
      "Number of frames of separation for difference "
      "calculation. Queue of collected images must be twice this "
      "value before a three-frame difference can be "
      "calculated.",
      1 ),

    PARAM_DEFAULT(
      jitter_radius,
      int,
      "Radius of jitter displacement (pixels) expected in the "
      "image due to imperfect stabilization. The image "
      "differencing process will search for the lowest-magnitude "
      "difference in a neighborhood with radius equal to "
      "jitter_radius.",
      0 ),

    PARAM_DEFAULT(
      max_foreground_fract,
      double,
      "Specifies the maximum expected fraction of the scene "
      "that may contain foreground movers at any time. When the "
      "fraction of pixels determined to be in motion exceeds "
      "this value, the background model is assumed to be "
      "invalid (e.g., due to excessive camera motion) and is "
      "reset. The default value of 1 indicates that no checking "
      "is done.",
      1 ),

    PARAM_DEFAULT(
      max_foreground_fract_thresh,
      double,
      "To be used in conjunction with max_foreground_fract, this "
      "parameter defines the threshold for foreground in order "
      "to determine if the maximum fraction of foreground has "
      "been exceeded.",
      -1 ),

    PARAM_DEFAULT(
      debug_dir,
      std::string,
      "Output debug images to this directory.",
      "" )
  );
}
