// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <arrows/core/algo/keyframe_selector_basic.h>
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
TEST ( keyframe_selector_basic, create )
{
  using namespace kwiver::vital;

  plugin_manager::instance().load_all_plugins();

  EXPECT_NE( nullptr, create_algorithm< algo::keyframe_selection >( "basic" ) );
}

// ----------------------------------------------------------------------------
TEST ( keyframe_selector_basic, default_config )
{
  EXPECT_PLUGGABLE_IMPL(
    keyframe_selector_basic,
    "A simple implementation of keyframe selection based on statistics "
    "of KLT tracks",
    PARAM_DEFAULT(
      fraction_tracks_lost_to_necessitate_new_keyframe, float,
      "If this fraction of more of features is lost then select a new keyframe",
      0.3 ),
    PARAM_DEFAULT(
      keyframe_min_feature_count, size_t,
      "Minimum number of features required for a frame to become a keyframe",
      50 )
  );
}
