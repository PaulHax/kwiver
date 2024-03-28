// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <arrows/ocv/algo/feature_detect_extract_BRISK.h>
#include <arrows/ocv/algo/feature_detect_extract_ORB.h>
#include <arrows/ocv/algo/feature_detect_extract_SIFT.h>
#include <arrows/ocv/algo/feature_detect_extract_SURF.h>

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
TEST ( feature_detect_extract_BRISK, create )
{
  plugin_manager::instance().load_all_plugins();
  EXPECT_NE(
    nullptr, create_algorithm< algo::detect_features >( "ocv_BRISK" ) );
}

// ----------------------------------------------------------------------------
TEST ( feature_detect_extract_ORB, create )
{
  plugin_manager::instance().load_all_plugins();
  EXPECT_NE(
    nullptr, create_algorithm< algo::detect_features >( "ocv_ORB" ) );
}

// ----------------------------------------------------------------------------
TEST ( feature_detect_extract_SIFT, create )
{
  plugin_manager::instance().load_all_plugins();
  EXPECT_NE(
    nullptr, create_algorithm< algo::detect_features >( "ocv_SIFT" ) );
}

// ----------------------------------------------------------------------------
TEST ( feature_detect_extract_SURF, create )
{
  plugin_manager::instance().load_all_plugins();
  EXPECT_NE(
    nullptr, create_algorithm< algo::detect_features >( "ocv_SURF" ) );
}

// ----------------------------------------------------------------------------
TEST ( feature_detect_extract_BRISK, default_config )
{
  EXPECT_PLUGGABLE_IMPL(
    detect_features_BRISK,
    "OpenCV feature detection via the BRISK algorithm",

    PARAM_DEFAULT(
      threshold, int,
      "AGAST detection threshold score.",
      30 ),

    PARAM_DEFAULT(
      octaves, int,
      "detection octaves. Use 0 to do single scale.",
      3 ),

    PARAM_DEFAULT(
      pattern_scale, float,
      "apply this scale to the pattern used for sampling the "
      "neighbourhood of a keypoint.",
      1 )
  );
}

// ----------------------------------------------------------------------------
TEST ( feature_detect_extract_ORB, default_config )
{
  EXPECT_PLUGGABLE_IMPL(
    detect_features_ORB,
    "OpenCV feature detection via the ORB algorithm",

    PARAM_DEFAULT(
      n_features, int,
      "The maximum number of features to retain",
      500 ),

    PARAM_DEFAULT(
      scale_factor, float,
      "Pyramid decimation ratio, greater than 1. "
      "scaleFactor==2 means the classical pyramid, where each "
      "next level has 4x less pixels than the previous, but "
      "such a big scale factor will degrade feature matching "
      "scores dramatically. On the other hand, too close to 1 "
      "scale factor will mean that to cover certain scale "
      "range you will need more pyramid levels and so the "
      "speed will suffer.",
      1.2f ),

    PARAM_DEFAULT(
      n_levels, int,
      "The number of pyramid levels. The smallest level will "
      "have linear size equal to "
      "input_image_linear_size/pow(scale_factor, "
      "n_levels).",
      9 ),

    PARAM_DEFAULT(
      edge_threshold, int,
      "This is size of the border where the features are not "
      "detected. It should roughly match the patch_size "
      "parameter.",
      31 ),

    PARAM_DEFAULT(
      first_level, int,
      "It should be 0 in the current implementation.",
      0 ),

    PARAM_DEFAULT(
      wta_k, int,
      "The number of points that produce each element of the "
      "oriented BRIEF descriptor. The default value 2 "
      "means the BRIEF where we take a random point pair "
      "and compare their brightnesses, so we get 0/1 "
      "response. Other possible values are 3 and 4. For "
      "example, 3 means that we take 3 random points (of "
      "course, those point coordinates are random, but "
      "they are generated from the pre-defined seed, so "
      "each element of BRIEF descriptor is computed "
      "deterministically from the pixel rectangle), find "
      "point of maximum brightness and output index of "
      "the winner (0, 1 or 2). Such output will occupy 2 "
      "bits, and therefore it will need a special variant "
      "of Hamming distance, denoted as NORM_HAMMING2 (2 "
      "bits per bin). When WTA_K=4, we take 4 random "
      "points to compute each bin (that will also occupy "
      "2 bits with possible values 0, 1, 2 or 3).",
      2 ),

    PARAM_DEFAULT(
      score_type,
      int,
      "The default HARRIS_SCORE (value=" KWIVER_STRINGIFY(
        cv::ORB::HARRIS_SCORE ) ") "
                                "means that Harris algorithm is used to rank features (the score is "
                                "written to KeyPoint::score and is used to retain best n_features "
                                "features); FAST_SCORE (value="
      KWIVER_STRINGIFY( cv::ORB::FAST_SCORE ) ") is "
                                              "alternative value of the parameter that produces slightly less "
                                              "stable key-points, but it is a little faster to compute.",
      cv::ORB::HARRIS_SCORE ),

    PARAM_DEFAULT(
      patch_size, int,
      "Size of the patch used by the oriented BRIEF "
      "descriptor. Of course, on smaller pyramid layers "
      "the perceived image area covered by a feature will "
      "be larger.",
      31 )

#if KWIVER_OPENCV_VERSION_MAJOR >= 3
    ,
    PARAM_DEFAULT( fast_threshold, int, "Undocumented", 20 )
#endif
  );
}

// ----------------------------------------------------------------------------
TEST ( feature_detect_extract_SIFT, default_config )
{
  EXPECT_PLUGGABLE_IMPL(
    detect_features_SIFT,
    "OpenCV feature detection via the SIFT algorithm",
    PARAM_DEFAULT(
      n_features, int,
      "The number of best features to retain. The features "
      "are ranked by their scores (measured in SIFT algorithm "
      "as the local contrast", 0 ),
    PARAM_DEFAULT(
      n_octave_layers, int,
      "The number of layers in each octave. 3 is the value "
      "used in D. Lowe paper. The number of octaves is "
      "computed automatically from the image resolution.",
      3 ),
    PARAM_DEFAULT(
      contrast_threshold, double,
      "The contrast threshold used to filter out weak "
      "features in semi-uniform (low-contrast) regions. The "
      "larger the threshold, the less features are produced "
      "by the detector.",
      0.04 ),
    PARAM_DEFAULT(
      edge_threshold, int,
      "The threshold used to filter out edge-like features. "
      "Note that the its meaning is different from the "
      "contrast_threshold, i.e. the larger the "
      "edge_threshold, the less features are filtered out "
      "(more features are retained).", 10 ),
    PARAM_DEFAULT(
      sigma, double,
      "The sigma of the Gaussian applied to the input image "
      "at the octave #0. If your image is captured with a "
      "weak camera with soft lenses, you might want to reduce "
      "the number.", 1.6 )
  );
}

// ----------------------------------------------------------------------------
TEST ( feature_detect_extract_SURF, default_config )
{
  EXPECT_PLUGGABLE_IMPL(
    detect_features_SURF,
    "OpenCV feature detection via the SURF algorithm",

    PARAM_DEFAULT(
      hessian_threshold, double,
      "Threshold for hessian keypoint detector used in SURF",
      100 ),

    PARAM_DEFAULT(
      n_octaves, int,
      "Number of pyramid octaves the keypoint detector will "
      "use.", 4 ),

    PARAM_DEFAULT(
      n_octaves_layers, int,
      "Number of octave layers within each octave.",
      3 ),

    PARAM_DEFAULT(
      extended, bool,
      "Extended descriptor flag (true - use extended "
      "128-element descriptors; false - use 64-element "
      "descriptors).",
      false ),

    PARAM_DEFAULT(
      upright, bool,
      "Up-right or rotated features flag (true - do not "
      "compute orientation of features; false - "
      "compute orientation).",
      false )
  );
}
