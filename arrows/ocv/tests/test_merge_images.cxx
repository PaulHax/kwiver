// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <arrows/ocv/algo/merge_images.h>
#include <vital/plugin_management/pluggable_macro_testing.h>
#include <vital/plugin_management/plugin_manager.h>

#include <vital/algo/algorithm.txx>

#include <gtest/gtest.h>

using namespace kwiver::vital;
using namespace kwiver::arrows::ocv;

// ----------------------------------------------------------------------------
int
main( int argc, char** argv )
{
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}

// ----------------------------------------------------------------------------
TEST ( merge_images, create )
{
  plugin_manager::instance().load_all_plugins();
  EXPECT_NE(
    nullptr, create_algorithm< algo::merge_images >( "ocv" ) );
}

// ----------------------------------------------------------------------------
TEST ( merge_images, default_config )
{
  EXPECT_PLUGGABLE_IMPL(
    merge_images,
    "Merge two images into one using opencv functions.\n\n"
    "The channels from the first image are added to the "
    "output image first, followed by the channels from the "
    "second image. This implementation takes no configuration "
    "parameters."
  );
}
