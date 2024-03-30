// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <arrows/ocv/algo/analyze_tracks.h>

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
TEST ( analyze_tracks, create )
{
  plugin_manager::instance().load_all_plugins();
  EXPECT_NE(
    nullptr,
    create_algorithm< algo::analyze_tracks >( "ocv" ) );
}

// ----------------------------------------------------------------------------
TEST ( analyze_tracks, default_config )
{
  EXPECT_PLUGGABLE_IMPL(
    analyze_tracks,
    "Use OpenCV to analyze statistics of feature tracks.",
    PARAM_DEFAULT(
      output_summary, bool,
      "Output a summary descriptor of high-level properties.",
      true ),
    PARAM_DEFAULT(
      output_pt_matrix, bool,
      "Output a matrix showing details about the percentage of "
      "features tracked for every frame, from each frame to "
      "some list of frames in the past.",
      true ),
    PARAM_DEFAULT(
      frames_to_compare, std::string,
      "A comma seperated list of frame difference intervals we want "
      "to use for the pt matrix. For example, if \"1, 4\" the pt "
      "matrix will contain comparisons between the current frame and "
      "last frame in addition to four frames ago.",
      "1,5,10,50" ) );
}
