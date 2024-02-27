// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Header defining the track_features_augment_keyframes

#ifndef KWIVER_ARROWS_CORE_TRACK_FEATURES_AUGMENT_KEYFRAMES_H_
#define KWIVER_ARROWS_CORE_TRACK_FEATURES_AUGMENT_KEYFRAMES_H_

#include <vital/algo/track_features.h>
#include <vital/vital_config.h>

#include <arrows/core/kwiver_algo_core_export.h>

#include <vital/types/feature_track_set.h>

// Later check which of these can be removed
#include <vital/algo/algorithm.h>
#include <vital/algo/algorithm.txx>

#include <vital/algo/detect_features.h>
#include <vital/algo/extract_descriptors.h>
#include <vital/exceptions/image.h>

namespace kwiver {

namespace arrows {

namespace core {

/// A class to augment feature tracks on keyframes
///
/// This algorithm applies a feature detector/descriptor on the current frame if
/// it is marked as a keyframe and creates new track states from those features.
/// It does nothing if the current frame is not a keyframe.  These new track
/// states are not currently linked to previous states in this algorithm.
class KWIVER_ALGO_CORE_EXPORT track_features_augment_keyframes
  : public vital::algo::track_features
{
public:
  PLUGGABLE_IMPL(
    track_features_augment_keyframes,
    "If the current frame is a keyframe, detect and describe "
    "additional features and create new tracks on this frame.",
    PARAM(
      extractor, vital::algo::extract_descriptors_sptr,
      "Extractor" ),
    PARAM(
      extractor_name, vital::algo::extract_descriptors_sptr,
      "Extractor name" )
  )

  /// Destructor
  virtual ~track_features_augment_keyframes() noexcept;

  /// Check that the algorithm's currently configuration is valid
  ///
  /// This checks solely within the provided \c config_block and not against
  /// the current state of the instance. This isn't static for inheritence
  /// reasons.
  ///
  /// \param config  The config block to check configuration of.
  ///
  /// \returns true if the configuration check passed and false if it didn't.
  bool check_configuration( vital::config_block_sptr config ) const override;

  /// Augment existing tracks with additional features if a keyframe
  ///
  /// This special tracking algorithm runs an additional feature detector
  /// on frames which have been labeled as keyframes.  If the specified
  /// frame is a keyframe in the track set, additional features are detected,
  /// descriptors are extracted, and new track states are added on this frame.
  /// If the specified frame is not a keyframe the tracks are returned
  /// unchanged.
  ///
  /// This tracking algorithm currently does not link any of the newly added
  /// tracks states to previous track states.
  ///
  /// \throws image_size_mismatch_exception
  ///    When the given non-zero mask image does not match the size of the
  ///    dimensions of the given image data.
  ///
  /// \param [in] tracks the feature tracks from previous tracking steps
  /// \param [in] frame_number the frame number of the current frame
  /// \param [in] image_data the image pixels for the current frame
  /// \param [in] mask Optional mask image that uses positive values to denote
  ///                  regions of the input image to consider for feature
  ///                  tracking. An empty sptr indicates no mask (default
  ///                  value).
  /// \returns an updated set of feature tracks
  kwiver::vital::feature_track_set_sptr
  track(
    kwiver::vital::feature_track_set_sptr prev_tracks,
    kwiver::vital::frame_id_t frame_number,
    kwiver::vital::image_container_sptr image_data,
    kwiver::vital::image_container_sptr mask = {} ) const override;

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
