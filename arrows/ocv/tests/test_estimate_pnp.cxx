// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <arrows/ocv/algo/estimate_pnp.h>
#include <vital/algo/algorithm.txx>

#include <vital/plugin_management/pluggable_macro_testing.h>
#include <vital/plugin_management/plugin_manager.h>

#include <gtest/gtest.h>

using namespace kwiver::vital;
using namespace kwiver::arrows;

using ocv::estimate_pnp;

static constexpr double ideal_rotation_tolerance = 1e-6;
static constexpr double ideal_center_tolerance = 1e-6;
static constexpr double noisy_rotation_tolerance = 0.008;
static constexpr double noisy_center_tolerance = 0.05;
static constexpr double outlier_rotation_tolerance = 0.008;
static constexpr double outlier_center_tolerance = 0.05;

// ----------------------------------------------------------------------------
int
main( int argc, char** argv )
{
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}

// ----------------------------------------------------------------------------
TEST ( estimate_pnp, create )
{
  plugin_manager::instance().load_all_plugins();

  EXPECT_NE(
    nullptr,
    kwiver::vital::create_algorithm< kwiver::vital::algo::estimate_pnp >(
      "ocv" ) );
}

// ----------------------------------------------------------------------------
TEST ( estimate_pnp, default_config )
{
  EXPECT_PLUGGABLE_IMPL(
    estimate_pnp,
    "Estimate camera pose with perspective N point method",

    PARAM_DEFAULT(
      confidence_threshold, double,
      "Confidence that estimated matrix is correct, range (0.0, 1.0]",
      0.99 ),

    PARAM_DEFAULT(
      max_iterations, int,
      "maximum number of iterations to run PnP [1, INT_MAX]",
      10000 ) );
}

// ----------------------------------------------------------------------------
#include <arrows/tests/test_estimate_pnp.h>
