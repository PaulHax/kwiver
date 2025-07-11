// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <arrows/serialize/protobuf/algo/detected_object_set.h>
#include <arrows/serialize/protobuf/algo/detected_object_type.h>
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
TEST ( detected_object_set, create )
{
  plugin_manager::instance().load_all_plugins();

  EXPECT_NE(
    nullptr,
    create_algorithm< algo::data_serializer >(
      "kwiver:protobuf:detected_object_set" ) );
}

// ----------------------------------------------------------------------------
TEST ( detected_object_type, create )
{
  plugin_manager::instance().load_all_plugins();

  EXPECT_NE(
    nullptr,
    create_algorithm< algo::data_serializer >(
      "kwiver:protobuf:detected_object_type" ) );
}
