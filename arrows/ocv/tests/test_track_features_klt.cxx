// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <arrows/ocv/track_features_klt.h>

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
TEST ( track_features_klt, create )
{
  plugin_manager::instance().load_all_plugins();
  EXPECT_NE(
    nullptr, create_algorithm< algo::track_features >( "ocv_KLT" ) );
}

// ----------------------------------------------------------------------------
TEST ( track_features_klt, default_config )
{
  EXPECT_PLUGGABLE_IMPL(
    track_features_klt,
    "OpenCV Lucas Kanade feature tracker",

    PARAM_DEFAULT(
      redetect_frac_lost_threshold, float,
      "redetect if fraction of features tracked from last "
      "detection drops below this level", 0.7 ),

    PARAM_DEFAULT(
      grid_rows, int,
      "rows in feature distribution enforcing grid", 0 ),

    PARAM_DEFAULT(
      grid_cols, int,
      "colums in feature distribution enforcing grid", 0 ),

    PARAM_DEFAULT(
      new_feat_exclusionary_radius_image_fraction,
      float,
      "do not place new features any closer than this fraction of image min "
      "dimension to existing features", 0.01 ),

    PARAM_DEFAULT(
      win_size, int,
      "klt image patch side length (it's a square)", 41 ),

    PARAM_DEFAULT(
      max_pyramid_level, int,
      "maximum pyramid level used in klt feature tracking", 3 ),

    PARAM_DEFAULT(
      target_number_of_features, int,
      "number of features that detector tries to find.  May be "
      "more or less depending on image content.  The algorithm "
      "attempts to distribute this many features evenly across "
      "the image. If texture is locally weak few feautres may be "
      "extracted in a local area reducing the total detected "
      "feature count.", 2048 ),

    PARAM_DEFAULT(
      klt_path_l1_difference_thresh, int,
      "patches with average l1 difference greater than this threshold "
      "will be discarded.", 10 ),

    PARAM(
      feature_detector, vital::algo::detect_features_sptr,
      "feature_detector configuration" )

  );
}
