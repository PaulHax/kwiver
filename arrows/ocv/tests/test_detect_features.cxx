// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <arrows/ocv/detect_features_AGAST.h>

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
