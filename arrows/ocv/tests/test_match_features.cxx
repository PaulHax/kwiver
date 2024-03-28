// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <arrows/ocv/algo/match_features_bruteforce.h>
#include <arrows/ocv/algo/match_features_flannbased.h>
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
TEST ( match_features_bruteforce, create )
{
  plugin_manager::instance().load_all_plugins();
  EXPECT_NE(
    nullptr, create_algorithm< algo::match_features >( "ocv_brute_force" ) );
}

// ----------------------------------------------------------------------------
TEST ( match_features_flannbased, create )
{
  plugin_manager::instance().load_all_plugins();
  EXPECT_NE(
    nullptr, create_algorithm< algo::match_features >( "ocv_flann_based" ) );
}

// ----------------------------------------------------------------------------
TEST ( match_features_bruteforce, default_config )
{
  EXPECT_PLUGGABLE_IMPL(
    match_features_bruteforce,
    "OpenCV feature matcher using brute force matching (exhaustive search).",

    PARAM_DEFAULT(
      cross_check, bool,
      "Perform cross checking when finding matches to filter "
      "through only the consistent pairs. This is an "
      "alternative to the ratio test used by D. Lowe in the "
      "SIFT paper.",
      false ),

//    PARAM_DEFAULT(
//      norm_type, int,
//      std::string(
//        "normalization type enum value. this should be one of the enum
// values:" )
//      +
//      list_enum_values
//      ,
//      cv::NORM_L2 )

  );
}

// ----------------------------------------------------------------------------
TEST ( match_features_flannbased, default_config )
{
  EXPECT_PLUGGABLE_IMPL(
    match_features_flannbased,
    "OpenCV feature matcher using FLANN (Approximate Nearest Neighbors).",

    PARAM_DEFAULT(
      cross_check, bool,
      "If cross-check filtering should be performed.",
      true ),

    PARAM_DEFAULT(
      cross_check_k, int,
      "Number of neighbors to use when cross checking",
      1 ),

    PARAM_DEFAULT(
      binary_descriptors, bool,
      "if false assume float descriptors (use l2 kdtree). "
      "if true assume binary descriptors (use lsh).",
      false )
  );
}
