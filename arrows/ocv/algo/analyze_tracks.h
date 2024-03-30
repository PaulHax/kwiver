// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Header for OpenCV analyze_tracks algorithm

#ifndef KWIVER_ARROWS_OCV_ANALYZE_TRACKS_H_
#define KWIVER_ARROWS_OCV_ANALYZE_TRACKS_H_

#include <arrows/ocv/kwiver_algo_ocv_export.h>

#include <vital/algo/analyze_tracks.h>

namespace kwiver {

namespace arrows {

namespace ocv {

/// A class for outputting various debug info about feature tracks
class KWIVER_ALGO_OCV_EXPORT analyze_tracks
  : public vital::algo::analyze_tracks
{
public:
  PLUGGABLE_IMPL(
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

  /// Destructor
  virtual ~analyze_tracks();

  /// Check that the algorithm's currently configuration is valid
  bool check_configuration( vital::config_block_sptr config ) const override;
  /// Output various information about the tracks stored in the input set.
  ///
  /// \param [in] track_set the tracks to analyze
  /// \param [in] stream an output stream to write data onto
  void
  print_info(
    vital::track_set_sptr track_set,
    stream_t& stream = std::cout ) const override;
};

} // end namespace ocv

} // end namespace arrows

} // end namespace kwiver

#endif
