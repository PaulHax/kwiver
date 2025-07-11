// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <arrows/core/algo/video_input_metadata_filter.h>
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
TEST ( video_input_metadata_filter, create )
{
  plugin_manager::instance().load_all_plugins();

  EXPECT_NE(
    nullptr,
    create_algorithm< algo::video_input >( "metadata_filter" ) );
}

// ----------------------------------------------------------------------------
TEST ( video_input_metadata_filter, default_config )
{
  EXPECT_PLUGGABLE_IMPL(
    video_input_metadata_filter,
    "A video input that calls another video input"
    " and applies a filter to the output metadata.",
    PARAM(
      video_input, vital::algo::video_input_sptr,
      "Algorithm pointer to video input" ),
    PARAM(
      metadata_filter,
      vital::algo::metadata_filter_sptr,
      "Algorithm pointer to metadata filter" )
  );
}
