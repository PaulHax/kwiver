// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <arrows/ocv/algo/resection_camera.h>

#include <vital/plugin_management/pluggable_macro_testing.h>
#include <vital/plugin_management/plugin_manager.h>

#include <vital/algo/algorithm.txx>

#include <gtest/gtest.h>

using namespace kwiver::vital;
using namespace kwiver::arrows;
using namespace std;

using ocv::resection_camera;

static constexpr double
  ideal_rotation_tolerance = 1e-6,
  ideal_center_tolerance = 1e-6,
  noisy_rotation_tolerance = 0.01,
  noisy_center_tolerance = 0.06;

// ----------------------------------------------------------------------------
int
main( int argc, char** argv )
{
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}

// ----------------------------------------------------------------------------
TEST ( resection_camera, create )
{
  plugin_manager::instance().load_all_plugins();
  EXPECT_NE(
    nullptr,
    kwiver::vital::create_algorithm< algo::resection_camera >( "ocv" ) );
}

// ----------------------------------------------------------------------------
TEST ( resection_camera, default_config )
{
  EXPECT_PLUGGABLE_IMPL(
    resection_camera,
    "resection camera using OpenCV calibrate camera method",
    PARAM(
      camera_options,
      resection_camera_options_sptr,
      "camera_options" )
  );
}

// ----------------------------------------------------------------------------
#include <arrows/tests/test_resection_camera.h>
