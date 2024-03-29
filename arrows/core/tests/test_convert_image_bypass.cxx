// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <arrows/core/algo/convert_image_bypass.h>
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
TEST ( convert_image_bypass, create )
{
  plugin_manager::instance().load_all_plugins();

  EXPECT_NE( nullptr, create_algorithm< algo::convert_image >( "bypass" ) );
}

// ----------------------------------------------------------------------------
TEST ( convert_image_bypass, default_config )
{
  EXPECT_PLUGGABLE_IMPL(
    convert_image_bypass,
    "Performs no conversion and returns the given image container."
  );
}
