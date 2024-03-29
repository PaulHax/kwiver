// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <arrows/core/algo/handle_descriptor_request_core.h>
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
TEST ( handle_descriptor_request_core, create )
{
  plugin_manager::instance().load_all_plugins();

  EXPECT_NE(
    nullptr,
    create_algorithm< algo::handle_descriptor_request >( "core" ) );
}

// ----------------------------------------------------------------------------
TEST ( handle_descriptor_request_core, default_config )
{
  EXPECT_PLUGGABLE_IMPL(
    handle_descriptor_request_core,
    "Formulate descriptors for later queries.",
    PARAM(
      image_reader, vital::algo::image_io_sptr,
      "image_reader" ),
    PARAM(
      descriptor_extractor, vital::algo::compute_track_descriptors_sptr,
      "descriptor_extractor" )
  );
}
