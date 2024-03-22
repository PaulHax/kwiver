// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Header defining the close_loops_exhaustive algorithm

#ifndef KWIVER_ARROWS__CLOSE_LOOPS_EXHAUSTIVE_H_
#define KWIVER_ARROWS__CLOSE_LOOPS_EXHAUSTIVE_H_

#include <arrows/core/kwiver_algo_core_export.h>

#include <vital/algo/algorithm.h>
#include <vital/types/feature_track_set.h>
#include <vital/types/image_container.h>

#include <vital/algo/algorithm.txx>
#include <vital/algo/close_loops.h>
#include <vital/config/config_block.h>

#include <vital/algo/match_features.h>

namespace kwiver {

namespace arrows {

namespace core {

/// Attempts to stitch over previous frames.
///
/// This class attempts close loops with all previous (or as specified) frames
class KWIVER_ALGO_CORE_EXPORT close_loops_exhaustive
  : public vital::algo::close_loops
{
public:
  PLUGGABLE_IMPL(
    close_loops_exhaustive,
    "Exhaustive matching of all frame pairs, "
    "or all frames within a moving window.",
    PARAM_DEFAULT(
      match_req, size_t,
      "The required number of features needed to be matched for a success.",
      100 ),
    PARAM_DEFAULT(
      num_look_back, int,
      "Maximum number of frames to search in the past for matching to "
      "(-1 looks back to the beginning).",
      -1 ),
    PARAM(
      feature_matcher, kwiver::vital::algo::match_features_sptr,
      "feature_matcher" )
  )

  /// Destructor
  virtual ~close_loops_exhaustive() noexcept;

  /// Check that the algorithm's currently configuration is valid
  ///
  /// This checks solely within the provided \c config_block and not against
  /// the current state of the instance. This isn't static for inheritence
  /// reasons.
  ///
  /// \param config  The config block to check configuration of.
  ///
  /// \returns true if the configuration check passed and false if it didn't.
  virtual bool check_configuration( vital::config_block_sptr config ) const;

  /// Perform exhaustive stitching
  ///
  /// \param [in] frame_number the frame number of the current frame
  /// \param [in] input the input feature track set to stitch
  /// \param [in] image image data for the current frame
  /// \param [in] mask Optional mask image where positive values indicate
  ///                  regions to consider in the input image.
  /// \returns an updated set of feature tracks after the stitching operation
  virtual vital::feature_track_set_sptr
  stitch(
    vital::frame_id_t frame_number,
    vital::feature_track_set_sptr input,
    vital::image_container_sptr image,
    vital::image_container_sptr mask = vital::image_container_sptr() ) const;

private:
  void initialize() override;
  /// private implementation class
  class priv;
  KWIVER_UNIQUE_PTR( priv, d_ );
};

} // end namespace core

} // end namespace arrows

} // end namespace kwiver

#endif
