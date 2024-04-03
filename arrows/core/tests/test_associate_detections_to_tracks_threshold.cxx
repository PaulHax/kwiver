// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <arrows/core/algo/associate_detections_to_tracks_threshold.h>
#include <vital/plugin_management/pluggable_macro_testing.h>
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
TEST ( associate_detections_to_tracks_threshold, create )
{
  using namespace kwiver::vital;

  plugin_manager::instance().load_all_plugins();

  EXPECT_NE(
    nullptr,
    create_algorithm< algo::associate_detections_to_tracks >( "threshold" ) );
}

// ----------------------------------------------------------------------------
TEST ( associate_detections_to_tracks_threshold, default_config )
{
  using namespace kwiver::vital;

  EXPECT_PLUGGABLE_IMPL(
    associate_detections_to_tracks_threshold,
    "Associate detections to tracks via simple thresholding on the input matrix.",
    PARAM_DEFAULT(
      threshold, double,
      "Threshold to apply on the matrix.",
      0.50 ),
    PARAM_DEFAULT(
      higher_is_better, bool,
      "Whether values above or below the threshold indicate a better fit.",
      true )
  );
}
