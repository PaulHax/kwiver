// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <arrows/ocv/algo/draw_tracks.h>

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
TEST ( draw_tracks, create )
{
  plugin_manager::instance().load_all_plugins();
  EXPECT_NE(
    nullptr, create_algorithm< algo::draw_tracks >( "ocv" ) );
}

// ----------------------------------------------------------------------------
TEST ( draw_tracks, default_config )
{
  EXPECT_PLUGGABLE_IMPL(
    draw_tracks,
    "Use OpenCV to draw tracked features on the images.",

    PARAM_DEFAULT(
      draw_track_ids,
      bool,
      "Draw track ids next to each feature point.",
      true ),

    PARAM_DEFAULT(
      draw_untracked_features,
      bool,
      "Draw untracked feature points in error_color.",
      true ),

    PARAM_DEFAULT(
      draw_match_lines,
      bool,
      "Draw lines between tracked features on the current frame "
      "to any past frames.",
      false ),

    PARAM_DEFAULT(
      draw_shift_lines,
      bool,
      "Draw lines showing the movement of the feature in the image "
      "plane from the last frame to the current one drawn on every "
      "single image individually.",
      false ),

    PARAM_DEFAULT(
      draw_comparison_lines,
      bool,
      "If more than 1 track set is input to this class, should we "
      "draw comparison lines between tracks with the same ids in "
      "both input sets?",
      true ),

    PARAM_DEFAULT(
      swap_comparison_set,
      bool,
      "If we are using a comparison track set, swap it and the input "
      "track set, so that the comparison set becomes the main set "
      "being displayed.",
      false ),

    PARAM_DEFAULT(
      write_images_to_disk,
      bool,
      "Should images be written out to disk?",
      true ),

    PARAM_DEFAULT(
      pattern,
      std::string,
      "The output pattern for writing images to disk.",
      "feature_tracks_%05d.png" ),

    PARAM_DEFAULT(
      past_frames_to_show,
      std::string,
      "A comma seperated list of past frames to show. For example: "
      "a value of \"3, 1\" will cause the GUI to generate a window "
      "3 frames wide, with the first frame being 2 frames behind the "
      "current frame, the second 1 frame behind, and the third being "
      "the current frame.",
      "" )
  );
}
