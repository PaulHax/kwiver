// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Header for OCV draw_tracks algorithm

#ifndef KWIVER_ARROWS_OCV_DRAW_TRACKS_H_
#define KWIVER_ARROWS_OCV_DRAW_TRACKS_H_

#include <arrows/ocv/kwiver_algo_ocv_export.h>

#include <vital/algo/draw_tracks.h>

namespace kwiver {

namespace arrows {

namespace ocv {

/// A class for drawing various information about feature tracks
class KWIVER_ALGO_OCV_EXPORT draw_tracks
  : public vital::algo::draw_tracks
{
public:
  PLUGGABLE_IMPL(
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

  /// Destructor
  virtual ~draw_tracks();

  bool check_configuration( vital::config_block_sptr config ) const override;

  /// Draw features tracks on top of the input images.
  ///
  /// This process can either be called in an offline fashion, where all
  /// tracks and images are provided to the function on the first call,
  /// or in an online fashion where only new images are provided on
  /// sequential calls. This function can additionally consumes a second
  /// track set for which can optionally be used to display additional
  /// information to provide a comparison between the two track sets.
  ///
  /// \param [in] display_set the main track set to draw
  /// \param [in] image_data a list of images the tracks were computed over
  /// \param [in] comparison_set optional comparison track set
  /// \returns a pointer to the last image generated
  vital::image_container_sptr
  draw(
    vital::track_set_sptr display_set,
    vital::image_container_sptr_list image_data,
    vital::track_set_sptr comparison_set = vital::track_set_sptr() ) override;

private:
  void initialize() override;
  /// private implementation class
  class priv;

  KWIVER_UNIQUE_PTR( priv, d );
};

} // end namespace ocv

} // end namespace arrows

} // end namespace kwiver

#endif
