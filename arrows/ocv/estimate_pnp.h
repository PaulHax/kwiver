// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief OCV estimate_pnp algorithm impl interface

#ifndef KWIVER_ARROWS_OCV_ESTIMATE_PNP_H_
#define KWIVER_ARROWS_OCV_ESTIMATE_PNP_H_

#include <arrows/ocv/kwiver_algo_ocv_export.h>
#include <vital/vital_config.h>

#include <vital/algo/estimate_pnp.h>

namespace kwiver {

namespace arrows {

namespace ocv {

/// A class that uses OpenCV to estimate a camera's pose from 3D feature
/// and point projection pairs.
class KWIVER_ALGO_OCV_EXPORT estimate_pnp
  : public vital::algo::estimate_pnp
{
public:
  PLUGGABLE_IMPL(
    estimate_pnp,
    "Estimate camera pose with perspective N point method",

    PARAM_DEFAULT(
      confidence_threshold, double,
      "Confidence that estimated matrix is correct, range (0.0, 1.0]",
      0.99 ),

    PARAM_DEFAULT(
      max_iterations, int,
      "maximum number of iterations to run PnP [1, INT_MAX]",
      10000 ) );

  /// Destructor
  virtual ~estimate_pnp();

  /// Check that the algorithm's configuration config_block is valid
  bool check_configuration( vital::config_block_sptr config ) const override;

  /// Estimate the camera's pose from the 3D points and their corresponding
  /// projections
  ///
  /// \param [in]  pts2d 2d projections of pts3d in the same order as pts3d
  /// \param [in]  pts3d 3d landmarks in the same order as pts2d.  Both must be
  /// same size.
  /// \param [in]  cal the intrinsic parameters of the camera
  /// \param [out] inliers for each point, the value is true if
  ///                     this pair is an inlier to the estimate
  kwiver::vital::camera_perspective_sptr
  estimate(
    const std::vector< vital::vector_2d >& pts2d,
    const std::vector< vital::vector_3d >& pts3d,
    const kwiver::vital::camera_intrinsics_sptr cal,
    std::vector< bool >& inliers ) const override;
};

} // end namespace ocv

} // end namespace arrows

} // end namespace kwiver

#endif
