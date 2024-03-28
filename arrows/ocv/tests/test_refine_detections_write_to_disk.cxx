// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <arrows/ocv/refine_detections_write_to_disk.h>

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
TEST ( refine_detections_write_to_disk, create )
{
  plugin_manager::instance().load_all_plugins();
  EXPECT_NE(
    nullptr, create_algorithm< algo::refine_detections >( "ocv" ) );
}

// ----------------------------------------------------------------------------
TEST ( refine_detections_write_to_disk, default_config )
{
  EXPECT_PLUGGABLE_IMPL(
    refine_detections_write_to_disk,
    "Debugging process for writing out detections",
    PARAM_DEFAULT(
      pattern, std::string,
      "The output pattern for writing images to disk. "
      "Parameters that may be included in the pattern are (in formatting order)"
      "the id (an integer), the source image filename (a string), "
      "and four values for the chip coordinate: "
      "top left x, top left y, width, height (all floating point numbers). "
      "A possible full pattern would be '%d-%s-%f-%f-%f-%f.png'. "
      "The pattern must contain the correct file extension.",
      "detection_%10d.png" )
  );
}
