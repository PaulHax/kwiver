// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <arrows/core/algo/estimate_canonical_transform.h>
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
TEST ( estimate_canonical_transform, create )
{
  plugin_manager::instance().load_all_plugins();

  EXPECT_NE(
    nullptr,
    create_algorithm< algo::estimate_canonical_transform >( "core_pca" ) );
}

// ----------------------------------------------------------------------------
TEST ( estimate_canonical_transform, default_config )
{
  EXPECT_PLUGGABLE_IMPL(
    estimate_canonical_transform,
    "Uses PCA to estimate a canonical similarity transform"
    " that aligns the best fit plane to Z=0",
    PARAM_DEFAULT(
      estimate_scale, bool,
      "Estimate the scale to normalize the data. "
      "If disabled the estimate transform is rigid",
      true ),
    PARAM_DEFAULT(
      height_percentile, double,
      "Shift the ground plane along the normal axis such that "
      "this percentage of landmarks are below the ground. Values "
      "are in the range [0.0, 1.0).  If the value is outside "
      "this range use the mean height instead.",
      0.05 )
  );
}
