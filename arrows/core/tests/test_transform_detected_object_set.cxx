// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <arrows/core/algo/transform_detected_object_set.h>
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
TEST ( transform_detected_object_set, create )
{
  plugin_manager::instance().load_all_plugins();

  EXPECT_NE(
    nullptr, create_algorithm< algo::detected_object_filter >(
      "transform_detected_object_set" ) );
}

// ----------------------------------------------------------------------------
TEST ( transform_detected_object_set, default_config )
{
  EXPECT_PLUGGABLE_IMPL(
    transform_detected_object_set,
    "Transforms a detected object set based on source and "
    "destination cameras.\n\n",
    PARAM_DEFAULT(
      src_camera_krtd_file_name, std::string,
      "Source camera KRTD file name path",
      "" ),
    PARAM_DEFAULT(
      dest_camera_krtd_file_name, std::string,
      "Destination camera KRTD file name path",
      "" )
  );
}
