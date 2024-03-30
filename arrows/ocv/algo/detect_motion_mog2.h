// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief OCV motion detection mog2 algorithm impl interface

#ifndef KWIVER_ARROWS_OCV_DETECT_MOTION_MOG2_H_
#define KWIVER_ARROWS_OCV_DETECT_MOTION_MOG2_H_

#include <memory>

#include <opencv2/opencv.hpp>

#include <vital/algo/detect_motion.h>
#include <vital/config/config_block.h>
#include <vital/types/timestamp.h>
#include <vital/vital_config.h>

#include <arrows/ocv/kwiver_algo_ocv_export.h>

namespace kwiver {

namespace arrows {

namespace ocv {

/// OCV implementation of detect_motion_using cv::BackgroundSubtractormog2
class KWIVER_ALGO_OCV_EXPORT detect_motion_mog2
  : public vital::algo::detect_motion
{
public:
  PLUGGABLE_IMPL(
    detect_motion_mog2,
    "OCV implementation of detect_motion using cv::BackgroundSubtractormog2",

    PARAM_DEFAULT(
      var_threshold,
      double,
      "Threshold on the squared Mahalanobis distance between "
      "the pixel and the model to decide whether a pixel is "
      "well described by the background model. This parameter "
      "does not affect the background update.",
      36.0 ),

    PARAM_DEFAULT(
      history,
      int,
      "Length of the history.",
      100 ),

    PARAM_DEFAULT(
      learning_rate,
      double,
      "determines how quickly features are “forgotten” from "
      "histograms (range 0-1).",
      0.01 ),

    PARAM_DEFAULT(
      blur_kernel_size,
      int,
      "Diameter of the normalized box filter blurring "
      "kernel (positive integer).",
      3 ),

    PARAM_DEFAULT(
      min_frames,
      int,
      "Minimum frames that need to be included in the "
      "background model before detections are emmited.",
      1 ),

    PARAM_DEFAULT(
      max_foreground_fract,
      double,
      "Specifies the maximum expected fraction of the scene "
      "that may contain foreground movers at any time. When the "
      "fraction of pixels determined to be in motion exceeds "
      "this value, the background model is assumed to be "
      "invalid (e.g., due to excessive camera motion) and is "
      "reset. The default value of 1 indicates that no checking "
      "is done.",
      1 )
  )

  /// Destructor
  virtual ~detect_motion_mog2() noexcept;

  /// Check that the algorithm's configuration vital::config_block is valid
  bool check_configuration( vital::config_block_sptr config ) const override;

  /// Detect motion from a sequence of images
  ///
  /// This method detects motion of foreground objects within a
  /// sequence of images in which the background remains stationary.
  /// Sequential images are passed one at a time. Motion estimates
  /// are returned for each image as a heat map with higher values
  /// indicating greater confidence.
  ///
  /// \param ts Timestamp for the input image
  /// \param image Image from a sequence
  /// \param reset_model Indicates that the background model should
  /// be reset, for example, due to changes in lighting condition or
  /// camera pose
  ///
  /// \returns A heat map image is returned indicating the confidence
  /// that motion occurred at each pixel. Heat map image is single channel
  /// and has the same width and height dimensions as the input image.
  kwiver::vital::image_container_sptr
  process_image(
    const kwiver::vital::timestamp& ts,
    const kwiver::vital::image_container_sptr image,
    bool reset_model ) override;

private:
  void initialize() override;
  void set_configuration_internal( vital::config_block_sptr config ) override;
  // private implementation class
  class priv;

  KWIVER_UNIQUE_PTR( priv, d_ );
};

} // end namespace ocv

} // end namespace arrows

} // end namespace kwiver

#endif
