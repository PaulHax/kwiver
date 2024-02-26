// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief OCV resection_camera algorithm impl interface

#ifndef KWIVER_ARROWS_OCV_RESECTION_CAMERA_H_
#define KWIVER_ARROWS_OCV_RESECTION_CAMERA_H_

#include <arrows/ocv/kwiver_algo_ocv_export.h>

#include <arrows/mvg/camera_options.h>
#include <vital/algo/resection_camera.h>

#include <arrows/ocv/config_options_helpers.txx>
#include <arrows/ocv/resection_camera_options.h>

namespace kwiver {

namespace arrows {

namespace ocv {

/// Use OpenCV to estimate a camera's pose and projection matrix from 3D
/// feature and point projection pairs.
class KWIVER_ALGO_OCV_EXPORT resection_camera
  : public vital::algo::resection_camera
{
public:
  PLUGGABLE_IMPL(
    resection_camera,
    "resection camera using OpenCV calibrate camera method",
    PARAM(
      camera_options,
      resection_camera_options_sptr,
      "camera_options" )
  );

  virtual ~resection_camera();

  /// Check that the algorithm's configuration config_block is valid.
  bool check_configuration( vital::config_block_sptr config ) const override;

  /// Estimate camera parameters from 3D points and their corresponding
  /// projections.
  kwiver::vital::camera_perspective_sptr
  resection(
    std::vector< kwiver::vital::vector_2d > const& image_points,
    std::vector< kwiver::vital::vector_3d > const& world_points,
    kwiver::vital::camera_intrinsics_sptr initial_calibration,
    std::vector< bool >* inliers ) const override;

  using vital::algo::resection_camera::resection;

private:
  void initialize() override;
};

} // namespace ocv

} // namespace arrows

} // namespace kwiver

#endif
