// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <arrows/serialize/protobuf/algo/track.h>
#include <arrows/serialize/protobuf/algo/track_set.h>
#include <arrows/serialize/protobuf/algo/track_state.h>
#include <vital/plugin_management/plugin_manager.h>

#include <gtest/gtest.h>

#include <algorithm>
#include <vital/algo/algorithm.txx>

using namespace kwiver::vital;
using namespace kwiver::arrows::serialize::protobuf;

// ----------------------------------------------------------------------------
int
main( int argc, char** argv )
{
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}

// ----------------------------------------------------------------------------
TEST ( track, create )
{
  plugin_manager::instance().load_all_plugins();

  EXPECT_NE(
    nullptr,
    create_algorithm< algo::data_serializer >( "kwiver:protobuf:track" ) );
}

// ----------------------------------------------------------------------------
TEST ( track_set, create )
{
  plugin_manager::instance().load_all_plugins();

  EXPECT_NE(
    nullptr,
    create_algorithm< algo::data_serializer >( "kwiver:protobuf:track_set" ) );
}

// ----------------------------------------------------------------------------
TEST ( track_state, create )
{
  plugin_manager::instance().load_all_plugins();

  EXPECT_NE(
    nullptr,
    create_algorithm< algo::data_serializer >(
      "kwiver:protobuf:track_state" ) );
}
