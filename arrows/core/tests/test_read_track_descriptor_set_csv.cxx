// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <arrows/core/algo/read_track_descriptor_set_csv.h>
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
TEST ( read_track_descriptor_set_csv, create )
{
  plugin_manager::instance().load_all_plugins();

  EXPECT_NE(
    nullptr,
    create_algorithm< algo::read_track_descriptor_set >( "csv" ) );
}

// ----------------------------------------------------------------------------
TEST ( read_track_descriptor_set_csv, default_config )
{
  EXPECT_PLUGGABLE_IMPL(
    read_track_descriptor_set_csv,
    "Track descriptor csv reader",
    PARAM_DEFAULT( batch_load, bool, "batch_load", true ),
    PARAM_DEFAULT( read_raw_descriptor, bool, "read_raw_descriptor", true ),
  );
}
