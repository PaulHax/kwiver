// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <arrows/ocv/algo/estimate_fundamental_matrix.h>
#include <vital/algo/algorithm.txx>

#include <vital/plugin_management/pluggable_macro_testing.h>
#include <vital/plugin_management/plugin_manager.h>

#include <gtest/gtest.h>

using namespace kwiver::vital;
using namespace kwiver::arrows::ocv;

static constexpr double ideal_tolerance = 3e-6;
static constexpr double outlier_tolerance = 0.01;

// ----------------------------------------------------------------------------
int
main( int argc, char** argv )
{
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}

// ----------------------------------------------------------------------------
TEST ( estimate_fundamental_matrix, create )
{
  plugin_manager::instance().load_all_plugins();

  EXPECT_NE(
    nullptr,
    create_algorithm< algo::estimate_fundamental_matrix >( "ocv" ) );
}

// ----------------------------------------------------------------------------
TEST ( estimate_fundamental_matrix, default_config )
{
  EXPECT_PLUGGABLE_IMPL(
    estimate_fundamental_matrix,
    "Use OpenCV to estimate a fundimental matrix from feature matches.",

    PARAM_DEFAULT(
      confidence_threshold,
      double,
      "Confidence that estimated matrix is correct, range (0.0, 1.0]",
      0.99 )
  );
}
// ----------------------------------------------------------------------------
#include <arrows/tests/test_estimate_fundamental_matrix.h>
