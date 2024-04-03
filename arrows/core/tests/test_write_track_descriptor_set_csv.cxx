// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <arrows/core/algo/write_track_descriptor_set_csv.h>
#include <vital/plugin_management/pluggable_macro_testing.h>
#include <vital/plugin_management/plugin_manager.h>

#include <gtest/gtest.h>

#include <vital/algo/algorithm.txx>

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
TEST ( write_track_descriptor_set_csv, create )
{
  plugin_manager::instance().load_all_plugins();

  EXPECT_NE(
    nullptr,
    create_algorithm< algo::write_track_descriptor_set >( "csv" ) );
}

// ----------------------------------------------------------------------------
TEST ( write_track_descriptor_set_csv, default_config )
{
  EXPECT_PLUGGABLE_IMPL(
    write_track_descriptor_set_csv,
    "Track descriptor set csv writer.",
    PARAM_DEFAULT( write_raw_descriptor, bool, "write_raw_descriptor", true ),
    PARAM_DEFAULT( write_world_loc, bool, "write_world_loc", false )
  );
}
