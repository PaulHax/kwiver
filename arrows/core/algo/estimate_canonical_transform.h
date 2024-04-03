// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#ifndef KWIVER_ARROWS__ESTIMATE_CANONICAL_TRANSFORM_H_
#define KWIVER_ARROWS__ESTIMATE_CANONICAL_TRANSFORM_H_

#include <arrows/core/kwiver_algo_core_export.h>

#include <vital/algo/estimate_canonical_transform.h>

#include <vital/algo/algorithm.h>
#include <vital/algo/algorithm.txx>

/// \file
/// \brief Header defining the estimate_canonical_transform algorithm

namespace kwiver {

namespace arrows {

namespace core {

/// Algorithm for estimating a canonical transform for cameras and landmarks
///
///  A canonical transform is a repeatable transformation that can be recovered
///  from data.  In this case we assume at most a similarity transformation.
///  If data sets P1 and P2 are equivalent up to a similarity transformation,
///  then applying a canonical transform to P1 and separately a
///  canonical transform to P2 should bring the data into the same coordinates.
///
///  This implementation centers the data at the mean of the landmarks. It
///  orients the data using PCA on the landmarks such that the X-axis aligns
///  with the largest principal direction and the Z-axis aligns with the
///  smallest.  The data is oriented such that the positive Z axis points
///  toward the mean of the camera centers.  The scale is set to normalized the
///  landmarks to unit standard deviation.
class KWIVER_ALGO_CORE_EXPORT estimate_canonical_transform
  : public vital::algo::estimate_canonical_transform
{
public:
  PLUGGABLE_IMPL(
    estimate_canonical_transform,
    "Uses PCA to estimate a canonical similarity transform"
    " that aligns the best fit plane to Z=0",
    PARAM_DEFAULT(
      estimate_scale, bool,
      "Estimate the scale to normalize the data. "
      "If disabled the estimate transform is rigid",
      true ),
    PARAM_DEFAULT(
      height_percentile, double,
      "Shift the ground plane along the normal axis such that "
      "this percentage of landmarks are below the ground. Values "
      "are in the range [0.0, 1.0).  If the value is outside "
      "this range use the mean height instead.",
      0.05 )
  )

  /// Destructor
  virtual ~estimate_canonical_transform();

  /// Copy Constructor
  estimate_canonical_transform( const estimate_canonical_transform& other );

  /// Check that the algorithm's configuration config_block is valid
  virtual bool check_configuration( vital::config_block_sptr config ) const;

  /// Estimate a canonical similarity transform for cameras and points
  ///
  /// \param cameras The camera map containing all the cameras
  /// \param landmarks The landmark map containing all the 3D landmarks
  /// \throws algorithm_exception When the data is insufficient or degenerate.
  /// \returns An estimated similarity transform mapping the data to the
  ///          canonical space.
  /// \note This algorithm does not apply the transformation, it only estimates
  /// it.
  virtual kwiver::vital::similarity_d
  estimate_transform(
    kwiver::vital::camera_map_sptr const cameras,
    kwiver::vital::landmark_map_sptr const landmarks ) const;

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
