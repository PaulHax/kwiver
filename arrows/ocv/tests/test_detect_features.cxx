// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <arrows/ocv/detect_features_AGAST.h>
#include <arrows/ocv/detect_features_FAST.h>
#include <arrows/ocv/detect_features_GFTT.h>
#include <arrows/ocv/detect_features_MSD.h>
#include <arrows/ocv/detect_features_MSER.h>
#include <arrows/ocv/detect_features_simple_blob.h>
#include <arrows/ocv/detect_features_STAR.h>

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
TEST ( detect_features_AGAST, create )
{
  plugin_manager::instance().load_all_plugins();
  EXPECT_NE(
    nullptr, create_algorithm< algo::detect_features >( "ocv_AGAST" ) );
}

// ----------------------------------------------------------------------------
TEST ( detect_features_FAST, create )
{
  plugin_manager::instance().load_all_plugins();
  EXPECT_NE(
    nullptr, create_algorithm< algo::detect_features >( "ocv_FAST" ) );
}

// ----------------------------------------------------------------------------
TEST ( detect_features_GFTT, create )
{
  plugin_manager::instance().load_all_plugins();
  EXPECT_NE(
    nullptr, create_algorithm< algo::detect_features >( "ocv_GFTT" ) );
}

// ----------------------------------------------------------------------------
TEST ( detect_features_MSD, create )
{
  plugin_manager::instance().load_all_plugins();
  EXPECT_NE(
    nullptr, create_algorithm< algo::detect_features >( "ocv_MSD" ) );
}

// ----------------------------------------------------------------------------
TEST ( detect_features_MSER, create )
{
  plugin_manager::instance().load_all_plugins();
  EXPECT_NE(
    nullptr, create_algorithm< algo::detect_features >( "ocv_MSER" ) );
}

// ----------------------------------------------------------------------------
TEST ( detect_features_simple_blob, create )
{
  plugin_manager::instance().load_all_plugins();
  EXPECT_NE(
    nullptr, create_algorithm< algo::detect_features >( "ocv_simple_blob" ) );
}

// ----------------------------------------------------------------------------
TEST ( detect_features_STAR, create )
{
  plugin_manager::instance().load_all_plugins();
  EXPECT_NE(
    nullptr, create_algorithm< algo::detect_features >( "ocv_STAR" ) );
}

// ----------------------------------------------------------------------------
TEST ( detect_features_AGAST, default_config )
{
  using namespace kwiver::arrows::ocv;

  static const std::string list_agast_types;

  EXPECT_PLUGGABLE_IMPL(
    detect_features_AGAST,
    "OpenCV feature detection via the AGAST algorithm",

    PARAM_DEFAULT(
      threshold, int,
      "Integer threshold on difference between intensity of "
      "the central pixel and pixels of a circle around this "
      "pixel", 10 ),

    PARAM_DEFAULT(
      nonmax_suppression, bool,
      "if true, non-maximum suppression is applied to "
      "detected corners (keypoints)", true ),

// Need to resolve compile error
//    PARAM_DEFAULT(
//      type, int,
//      "Neighborhood pattern type. Should be one of the "
//      "following enumeration type values:\n" +
//      list_agast_types +  " (default)",
//      static_cast< int >( cv::AgastFeatureDetector::OAST_9_16 )
//    )
  );
}

// ----------------------------------------------------------------------------
TEST ( detect_features_FAST, default_config )
{
  using namespace kwiver::arrows::ocv;

  EXPECT_PLUGGABLE_IMPL(
    detect_features_FAST,
    "OpenCV feature detection via the FAST algorithm",

    PARAM_DEFAULT(
      threshold, int,
      "Integer threshold on difference between intensity of "
      "the central pixel and pixels of a circle around this "
      "pixel", 10 ),

    PARAM_DEFAULT(
      nonmaxSuppression, bool,
      "Integer threshold on difference between intensity of "
      "the central pixel and pixels of a circle around this "
      "pixel", true ),

    PARAM_DEFAULT(
      target_num_features_detected,  int,
      "algorithm tries to output approximately this many features. "
      "Disable by setting to negative value.", 2500 )

// Need to resolve the test errors in this block of code
#if KWIVER_OPENCV_VERSION_MAJOR >= 3
    ,
    PARAM_DEFAULT(
      neighborhood_type,
      int,
      "one of the three neighborhoods as defined in the paper: "
      "TYPE_5_8="  KWIVER_STRINGIFY( cv::FastFeatureDetector::TYPE_5_8 ) ","
                                                                         "TYPE_7_12="
      KWIVER_STRINGIFY( cv::FastFeatureDetector::TYPE_7_12 ) ", "
                                                             "TYPE_9_16="
      KWIVER_STRINGIFY( cv::FastFeatureDetector::TYPE_9_16 ) ".",
      int( cv::FastFeatureDetector::TYPE_9_16 ) )
#endif
  );
}

// ----------------------------------------------------------------------------
TEST ( detect_features_GFTT, default_config )
{
  using namespace kwiver::arrows::ocv;

  EXPECT_PLUGGABLE_IMPL(
    detect_features_GFTT,
    "OpenCV feature detection via the GFTT algorithm",
    PARAM_DEFAULT( max_corners, int, "max_corners", 1000 ),
    PARAM_DEFAULT( quality_level, double, "quality_level", 0.01 ),
    PARAM_DEFAULT( min_distance, double, "min_distance", 1 ),
    PARAM_DEFAULT( block_size, int, "block_size", 3 ),
    PARAM_DEFAULT( use_harris_detector, bool, "use_harris_detector", false ),
    PARAM_DEFAULT( k, double, "k", 0.04 )
  );
}

// ----------------------------------------------------------------------------
TEST ( detect_features_MSD, default_config )
{
  using namespace kwiver::arrows::ocv;

  EXPECT_PLUGGABLE_IMPL(
    detect_features_MSD,
    "OpenCV feature detection via the MSD algorithm",
    PARAM_DEFAULT( patch_radius, int, "patch_radius", 3 ),
    PARAM_DEFAULT( search_area_radius, int, "search_area_radius", 5 ),
    PARAM_DEFAULT( nms_radius, int, "nms_radius", 5 ),
    PARAM_DEFAULT( nms_scale_radius, int, "nms_scale_radius", 0 ),
    PARAM_DEFAULT( th_saliency, float, "th_saliency", 250 ),
    PARAM_DEFAULT( knn, int, "knn", 4 ),
    PARAM_DEFAULT( scale_factor, float, "scale_factor", 1.25 ),
    PARAM_DEFAULT( n_scales, int, "n_scales", -1 ),
    PARAM_DEFAULT( compute_orientation, bool, "compute_orientation", false )
  );
}

// ----------------------------------------------------------------------------
TEST ( detect_features_MSER, default_config )
{
  using namespace kwiver::arrows::ocv;

  EXPECT_PLUGGABLE_IMPL(
    detect_features_MSER,
    "OpenCV feature detection via the MSER algorithm",

    PARAM_DEFAULT(
      delta,
      int,
      "Compares (size[i] - size[i-delta]) / size[i-delta]",
      5 ),
    PARAM_DEFAULT(
      min_area,
      int,
      "Prune areas smaller than this",
      60 ),
    PARAM_DEFAULT(
      max_area,
      int,
      "Prune areas larger than this",
      14400 ),
    PARAM_DEFAULT(
      max_variation,
      double,
      "Prune areas that have similar size to its children",
      0.25 ),
    PARAM_DEFAULT(
      min_diversity,
      double,
      "For color images, trace back to cut off MSER with "
      "diversity less than min_diversity",
      0.2 ),
    PARAM_DEFAULT(
      max_evolution,
      int,
      "The color images, the evolution steps.",
      200 ),
    PARAM_DEFAULT(
      area_threshold,
      double,
      "For color images, the area threshold to cause "
      "re-initialization",
      1.01 ),
    PARAM_DEFAULT(
      min_margin,
      double,
      "For color images, ignore too-small regions.",
      0.003 ),
    PARAM_DEFAULT(
      edge_blur_size,
      int,
      "For color images, the aperture size for edge blur",
      5 )
#if KWIVER_OPENCV_VERSION_MAJOR >= 3
    ,
    PARAM_DEFAULT(
      pass2only,
      bool,
      "Undocumented",
      false )
#endif
  );
}

// ----------------------------------------------------------------------------
TEST ( detect_features_simple_blob, default_config )
{
  using namespace kwiver::arrows::ocv;

  EXPECT_PLUGGABLE_IMPL(
    detect_features_simple_blob,
    "OpenCV feature detection via the simple_blob algorithm.",

    PARAM_DEFAULT(
      threshold_step,
      float,
      "Defines stepping between min and max threshold when "
      "converting the source image to binary images by "
      "applying thresholding with several thresholds from "
      "min_threshold (inclusive) to max_threshold (exclusive) "
      " with distance threshold_step between neighboring "
      "thresholds.",
      default_params.thresholdStep ),
    PARAM_DEFAULT(
      threshold_min,
      float,
      "threshold_min",
      default_params.minThreshold ),
    PARAM_DEFAULT(
      threshold_max,
      float,
      "threshold_max",
      default_params.maxThreshold ),
    PARAM_DEFAULT(
      min_repeatability,
      std::size_t,
      "min_repeatability",
      default_params.minRepeatability ),
    PARAM_DEFAULT(
      min_dist_between_blocks,
      float,
      "Close centers form one group that corresponds to one "
      "blob, controlled by this distance value.",
      default_params.minDistBetweenBlobs ),
    PARAM_DEFAULT(
      filter_by_color,
      bool,
      "Enable blob filtering by intensity of the binary image "
      "at the center of the blob to blob_color. If they "
      "differ, the blob is filtered out. Use blob_color = 0 "
      "to extract dark blobs and blob_color = 255 to extract "
      "light blobs",
      default_params.filterByColor ),
    PARAM_DEFAULT(
      blob_color,
      unsigned char,
      "blob_color",
      default_params.blobColor ),
    PARAM_DEFAULT(
      filter_by_area,
      bool,
      "Enable blob filtering by area to those between "
      "min_area (inclusive) and max_area (exclusive).",
      default_params.filterByArea ),
    PARAM_DEFAULT(
      min_area,
      float,
      "min_area",
      default_params.minArea ),
    PARAM_DEFAULT(
      max_area,
      float,
      "max_area",
      default_params.maxArea ),
    PARAM_DEFAULT(
      filter_by_circularity,
      bool,
      "Enable blob filtering by circularity to those between "
      "min_circularity (inclusive) and max_circularity "
      "(exclusive).",
      default_params.filterByCircularity ),
    PARAM_DEFAULT(
      min_circularity,
      float,
      "min_circularity",
      default_params.minCircularity ),
    PARAM_DEFAULT(
      max_circularity,
      float,
      "max_circularity",
      default_params.maxCircularity ),
    PARAM_DEFAULT(
      filter_by_inertia,
      bool,
      "Enable blob filtering by the ratio of inertia between "
      "min_inertia_ratio (inclusive) and max_inertia_ratio "
      "(exclusive).",
      default_params.filterByInertia ),
    PARAM_DEFAULT(
      min_inertia_ratio,
      float,
      "min_inertia_ratio",
      default_params.minInertiaRatio ),
    PARAM_DEFAULT(
      max_inertia_ratio,
      float,
      "max_inertia_ratio",
      default_params.maxInertiaRatio ),
    PARAM_DEFAULT(
      filter_by_convexity,
      bool,
      "Enable filtering by convexity where blobs have "
      "convexity (area / area of blob convex hull) between "
      "min_convexity (inclusive) and max_convexity "
      "(exclusive).",
      default_params.filterByConvexity ),
    PARAM_DEFAULT(
      min_convexity,
      float,
      "min_convexity",
      default_params.minConvexity ),
    PARAM_DEFAULT(
      max_convexity,
      float,
      "max_convexity",
      default_params.maxConvexity
    )
  );
}

// ----------------------------------------------------------------------------
TEST ( detect_features_STAR, default_config )
{
  using namespace kwiver::arrows::ocv;

  EXPECT_PLUGGABLE_IMPL(
    detect_features_STAR,
    "OpenCV feature detection via the STAR algorithm",

    PARAM_DEFAULT( max_size, int, "max_size", 45 ),
    PARAM_DEFAULT( response_threshold, int, "response_threshold", 30 ),
    PARAM_DEFAULT(
      line_threshold_projected, int, "line_threshold_projected",
      10 ),
    PARAM_DEFAULT(
      line_threshold_binarized, int, "line_threshold_binarized",
      8 ),
    PARAM_DEFAULT( suppress_nonmax_size, int, "suppress_nonmax_size", 5 )
  );
}
