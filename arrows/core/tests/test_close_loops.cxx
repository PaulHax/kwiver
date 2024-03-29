// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <arrows/core/algo/close_loops_appearance_indexed.h>
#include <arrows/core/algo/close_loops_bad_frames_only.h>
#include <arrows/core/algo/close_loops_exhaustive.h>
#include <arrows/core/algo/close_loops_keyframe.h>
#include <arrows/core/algo/close_loops_multi_method.h>
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
TEST ( close_loops_appearance_indexed, create )
{
  plugin_manager::instance().load_all_plugins();

  EXPECT_NE(
    nullptr, create_algorithm< algo::close_loops >(
      "appearance_indexed" ) );
}

// ----------------------------------------------------------------------------
TEST ( close_loops_bad_frames_only, create )
{
  plugin_manager::instance().load_all_plugins();

  EXPECT_NE(
    nullptr, create_algorithm< algo::close_loops >(
      "bad_frames_only" ) );
}

// ----------------------------------------------------------------------------
TEST ( close_loops_exhaustive, create )
{
  plugin_manager::instance().load_all_plugins();

  EXPECT_NE(
    nullptr, create_algorithm< algo::close_loops >(
      "exhaustive" ) );
}

// ----------------------------------------------------------------------------
TEST ( close_loops_keyframe, create )
{
  plugin_manager::instance().load_all_plugins();

  EXPECT_NE(
    nullptr, create_algorithm< algo::close_loops >(
      "keyframe" ) );
}

// ----------------------------------------------------------------------------
TEST ( close_loops_multi_method, create )
{
  plugin_manager::instance().load_all_plugins();

  EXPECT_NE(
    nullptr, create_algorithm< algo::close_loops >(
      "multi_method" ) );
}
