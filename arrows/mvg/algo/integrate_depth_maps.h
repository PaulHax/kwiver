// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/**
 * \file
 * \brief Header file for integration of depth maps
 */

#ifndef KWIVER_ARROWS_MVG_INTEGRATE_DEPTH_MAPS_H_
#define KWIVER_ARROWS_MVG_INTEGRATE_DEPTH_MAPS_H_

#include <arrows/mvg/kwiver_algo_mvg_export.h>

#include <vital/algo/algorithm.h>
#include <vital/algo/algorithm.txx>
#include <vital/algo/integrate_depth_maps.h>
#include <vital/types/vector.h>
#include <vital/vital_config.h>

namespace kwiver {

namespace arrows {

namespace mvg {

class KWIVER_ALGO_MVG_EXPORT integrate_depth_maps
  : public vital::algo::integrate_depth_maps
{
public:
  using array3 = std::array< double, 3 >;
  PLUGGABLE_IMPL(
    integrate_depth_maps,
    "CPU depth map fusion",
    PARAM_DEFAULT(
      ray_potential_thickness, double,
      "Distance that the TSDF covers sloping from Rho to zero. "
      "Units are in voxels.", 20.0 ),
    PARAM_DEFAULT(
      ray_potential_rho, double,
      "Maximum magnitude of the TDSF", 1.0 ),
    PARAM_DEFAULT(
      ray_potential_eta, double,
      "Fraction of rho to use for free space constraint. "
      "Requires 0 <= Eta <= 1.", 1.0 ),
    PARAM_DEFAULT(
      ray_potential_epsilon, double,
      "Fraction of rho to use in occluded space. "
      "Requires 0 <= Epsilon <= 1.", 0.01 ),
    PARAM_DEFAULT(
      ray_potential_delta, double,
      "Distance from the surface before the TSDF is truncate. "
      "Units are in voxels", 10.0 ),
    PARAM_DEFAULT(
      voxel_spacing_factor, double,
      "Multiplier on voxel spacing.  Set to 1.0 for voxel "
      "sizes that project to 1 pixel on average.", 1.0 ),
    PARAM_DEFAULT(
      grid_spacing, array3,
      "Relative spacing for each dimension of the grid",
      array3( { 1., 1., 1. } ) )
  )

  /// Destructor
  virtual ~integrate_depth_maps();

  void initialize() override;

  /// Check that the algorithm's currently configuration is valid
  virtual bool check_configuration( vital::config_block_sptr config ) const;

  /// Integrate multiple depth maps with per-pixel weights into a common volume
  ///
  /// The weight maps in this variant encode how much weight to give each depth
  /// pixel in the integration sum.  If the vector of weight_maps is empty then
  /// all depths are given full weight.
  ///
  /// \param [in]     minpt_bound the min point of the bounding region
  /// \param [in]     maxpt_bound the max point of the bounding region
  /// \param [in]     depth_maps  the set of floating point depth map images
  /// \param [in]     weight_maps the set of floating point [0,1] weight maps
  /// \param [in]     cameras     the set of cameras, one for each depth map
  /// \param [in,out] volume      the fused volumetric data
  /// \param [out]    spacing     the spacing between voxels in each dimension
  ///
  /// \note the volume data is stored as a 3D image.  Metadata fields on the
  /// image specify the origin and scale of the volume in world coordinates.
  virtual void
  integrate(
    kwiver::vital::vector_3d const& minpt_bound,
    kwiver::vital::vector_3d const& maxpt_bound,
    std::vector< kwiver::vital::image_container_sptr > const& depth_maps,
    std::vector< kwiver::vital::image_container_sptr > const& weight_maps,
    std::vector< kwiver::vital::camera_perspective_sptr > const& cameras,
    kwiver::vital::image_container_sptr& volume,
    kwiver::vital::vector_3d& spacing ) const;

private:
  /// private implementation class
  class priv;
  KWIVER_UNIQUE_PTR( priv, d_ );
};

}  // end namespace mvg

}  // end namespace arrows

}  // end namespace kwiver

#endif
