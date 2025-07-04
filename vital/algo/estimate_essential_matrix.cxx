// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief estimate essential matrix instantiation

#include <vital/algo/estimate_essential_matrix.h>

namespace kwiver {

namespace vital {

namespace algo {

estimate_essential_matrix
::estimate_essential_matrix()
{
  attach_logger( "algo.estimate_essential_matrix" );
}

/// Estimate an essential matrix from corresponding features
essential_matrix_sptr
estimate_essential_matrix
::estimate(
  feature_set_sptr feat1,
  feature_set_sptr feat2,
  match_set_sptr matches,
  const camera_intrinsics_sptr cal1,
  const camera_intrinsics_sptr cal2,
  std::vector< bool >& inliers,
  double inlier_scale ) const
{
  std::vector< feature_sptr > vf1 = feat1->features();
  std::vector< feature_sptr > vf2 = feat2->features();
  std::vector< match > mset = matches->matches();
  std::vector< vector_2d > vv1, vv2;

  for( match m : mset )
  {
    vv1.push_back( vf1[ m.first ]->loc() );
    vv2.push_back( vf2[ m.second ]->loc() );
  }
  return this->estimate( vv1, vv2, cal1, cal2, inliers, inlier_scale );
}

/// Estimate an essential matrix from corresponding features
essential_matrix_sptr
estimate_essential_matrix
::estimate(
  feature_set_sptr feat1,
  feature_set_sptr feat2,
  match_set_sptr matches,
  const camera_intrinsics_sptr cal,
  std::vector< bool >& inliers,
  double inlier_scale ) const
{
  return this->estimate(
    feat1, feat2, matches, cal, cal, inliers,
    inlier_scale );
}

/// Estimate an essential matrix from corresponding features
essential_matrix_sptr
estimate_essential_matrix
::estimate(
  const std::vector< vector_2d >& pts1,
  const std::vector< vector_2d >& pts2,
  const camera_intrinsics_sptr cal,
  std::vector< bool >& inliers,
  double inlier_scale ) const
{
  return this->estimate( pts1, pts2, cal, cal, inliers, inlier_scale );
}

} // namespace algo

} // namespace vital

} // namespace kwiver
