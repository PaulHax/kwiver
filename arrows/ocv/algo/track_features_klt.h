// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Header defining the OpenCV track_features algorithm using KLT

#ifndef ARROWS_PLUGINS_OCV_TRACK_FEATURES_KLT_H_
#define ARROWS_PLUGINS_OCV_TRACK_FEATURES_KLT_H_

#include <arrows/ocv/kwiver_algo_ocv_export.h>

#include <vital/algo/algorithm.txx>
#include <vital/algo/detect_features.h>
#include <vital/algo/track_features.h>
#include <vital/types/feature_track_set.h>
#include <vital/types/image_container.h>

namespace kwiver {

namespace arrows {

namespace ocv {

/// A basic feature tracker
class KWIVER_ALGO_OCV_EXPORT track_features_klt
  : public vital::algo::track_features
{
public:
  PLUGGABLE_IMPL(
    track_features_klt,
    "OpenCV Lucas Kanade feature tracker",

    PARAM_DEFAULT(
      redetect_frac_lost_threshold, float,
      "redetect if fraction of features tracked from last "
      "detection drops below this level", 0.7f ),

    PARAM_DEFAULT(
      grid_rows, int,
      "rows in feature distribution enforcing grid", 0 ),

    PARAM_DEFAULT(
      grid_cols, int,
      "colums in feature distribution enforcing grid", 0 ),

    PARAM_DEFAULT(
      new_feat_exclusionary_radius_image_fraction,
      float,
      "do not place new features any closer than this fraction of image min "
      "dimension to existing features", 0.01f ),

    PARAM_DEFAULT(
      win_size, int,
      "klt image patch side length (it's a square)", 41 ),

    PARAM_DEFAULT(
      max_pyramid_level, int,
      "maximum pyramid level used in klt feature tracking", 3 ),

    PARAM_DEFAULT(
      target_number_of_features, int,
      "number of features that detector tries to find.  May be "
      "more or less depending on image content.  The algorithm "
      "attempts to distribute this many features evenly across "
      "the image. If texture is locally weak few feautres may be "
      "extracted in a local area reducing the total detected "
      "feature count.", 2048 ),

    PARAM_DEFAULT(
      klt_path_l1_difference_thresh, int,
      "patches with average l1 difference greater than this threshold "
      "will be discarded.", 10 ),

    PARAM(
      feature_detector, vital::algo::detect_features_sptr,
      "feature_detector configuration" )
  )

  /// Destructor
  virtual ~track_features_klt() noexcept;

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

  /// Extend a previous set of feature tracks using the current frame
  ///
  /// \throws image_size_mismatch_exception
  ///    When the given non-zero mask image does not match the size of the
  ///    dimensions of the given image data.
  ///
  /// \param [in] prev_tracks the feature tracks from previous tracking steps
  /// \param [in] frame_number the frame number of the current frame
  /// \param [in] image_data the image pixels for the current frame
  /// \param [in] mask Optional mask image that uses positive values to denote
  ///                  regions of the input image to consider for feature
  ///                  tracking. An empty sptr indicates no mask (default
  ///                  value).
  /// \returns an updated set of feature tracks including the current frame
  vital::feature_track_set_sptr
  track(
    vital::feature_track_set_sptr prev_tracks,
    vital::frame_id_t frame_number,
    vital::image_container_sptr image_data,
    vital::image_container_sptr mask = {} ) const override;

private:
  void initialize() override;
  /// private implementation class
  class priv;
  KWIVER_UNIQUE_PTR( priv, d_ );
};

} // end namespace ocv

} // end namespace arrows

} // end namespace kwiver

#endif
