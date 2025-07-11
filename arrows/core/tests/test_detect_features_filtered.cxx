// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <arrows/core/algo/detect_features_filtered.h>
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
TEST ( detect_features_filtered, create )
{
  plugin_manager::instance().load_all_plugins();

  EXPECT_NE( nullptr, create_algorithm< algo::detect_features >( "filtered" ) );
}

// ----------------------------------------------------------------------------
TEST ( detect_features_filtered, default_config )
{
  EXPECT_PLUGGABLE_IMPL(
    detect_features_filtered,
    "Wrapper that runs a feature detector and "
    "applies a filter to the detector output",
    PARAM(
      detector, vital::algo::detect_features_sptr,
      "detector" ),
    PARAM(
      filter, vital::algo::filter_features_sptr,
      "filter" )
  );
}
