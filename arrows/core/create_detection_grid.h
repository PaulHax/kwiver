// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#ifndef _KWIVER_ARROWS_CREATE_DETECTION_GRID_H_
#define _KWIVER_ARROWS_CREATE_DETECTION_GRID_H_

#include <arrows/core/kwiver_algo_core_export.h>
#include <vital/vital_config.h>

#include <vital/algo/algorithm.h>
#include <vital/algo/algorithm.txx>
#include <vital/algo/image_object_detector.h>

namespace kwiver {

namespace arrows {

namespace core {

/// Initialize object tracks via simple single frame thresholding
class KWIVER_ALGO_CORE_EXPORT create_detection_grid
  : public vital::algo::image_object_detector
{
public:
  PLUGGABLE_IMPL(
    create_detection_grid,
    "Create a grid of detections across the input image.",
    PARAM_DEFAULT(
      width, double,
      "Width of each detection in the output grid.",
      0.0 ),
    PARAM_DEFAULT(
      height, double,
      "Height of each detection in the output grid.",
      0.0 ),
    PARAM_DEFAULT(
      x_step, double,
      "How far apart along the x axis each detection is.",
      0.0 ),
    PARAM_DEFAULT(
      y_step, double,
      "How far apart along the y axis each detection is.",
      0.0 )
  )

  /// Destructor
  virtual ~create_detection_grid() noexcept;

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

  /// Find all objects on the provided image
  ///
  /// This method analyzes the supplied image and along with any saved
  /// context, returns a vector of detected image objects.
  ///
  /// \param image_data the image pixels
  /// \returns vector of image objects found
  virtual vital::detected_object_set_sptr
  detect( vital::image_container_sptr image_data ) const;

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
