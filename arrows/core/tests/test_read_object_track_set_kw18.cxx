// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <arrows/core/algo/read_object_track_set_kw18.h>
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
TEST ( read_object_track_set_kw18, create )
{
  using namespace kwiver::vital;

  plugin_manager::instance().load_all_plugins();

  EXPECT_NE(
    nullptr,
    create_algorithm< algo::read_object_track_set >( "kw18" ) );
}

// ----------------------------------------------------------------------------
TEST ( read_object_track_set_kw18, default_config )
{
  EXPECT_PLUGGABLE_IMPL(
    read_object_track_set_kw18,
    "Object track set kw18 reader.",
    PARAM_DEFAULT( delim, std::string, "delimeter", " " ),
    PARAM_DEFAULT( batch_load, bool, "batch_load", true )
  );
}
