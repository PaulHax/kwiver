// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief OCV estimate_pnp algorithm implementation

#include <cmath>

#include "estimate_pnp.h"

#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/core/eigen.hpp>

#include "camera_intrinsics.h"

namespace kwiver {

namespace arrows {

namespace ocv {

/// Destructor
estimate_pnp
::~estimate_pnp()
{}

/// Check that the algorithm's configuration vital::config_block is valid
bool
estimate_pnp
::check_configuration( vital::config_block_sptr config ) const
{
  bool good_conf = true;
  double confidence_threshold =
    config->get_value< double >(
      "confidence_threshold",
      this->get_confidence_threshold() );

  if( confidence_threshold <= 0.0 || confidence_threshold > 1.0 )
  {
    LOG_ERROR(
      this->logger(), "confidence_threshold parameter is "
        << confidence_threshold
        << ", needs to be in (0.0, 1.0]." );
    good_conf = false;
  }

  int max_iterations =
    config->get_value< int >( "max_iterations", this->get_max_iterations() );

  if( max_iterations < 1 )
  {
    LOG_ERROR(
      this->logger(), "max iterations is " << max_iterations
                                           << ", needs to be greater than zero." );
    good_conf = false;
  }

  return good_conf;
}

/// Estimate a camera pose from corresponding points
vital::camera_perspective_sptr
estimate_pnp
::estimate(
  const std::vector< vital::vector_2d >& pts2d,
  const std::vector< vital::vector_3d >& pts3d,
  const kwiver::vital::camera_intrinsics_sptr cal,
  std::vector< bool >& inliers ) const
{
  if( pts2d.size() < 3 || pts3d.size() < 3 )
  {
    LOG_ERROR(
      this->logger(),
      "Not enough points to estimate camera's pose" );
    return vital::camera_perspective_sptr();
  }
  if( pts2d.size() != pts3d.size() )
  {
    LOG_ERROR(
      this->logger(),
      "Number of 3D points and projections should match.  They don't." );
  }

  std::vector< cv::Point2f > projs;
  std::vector< cv::Point3f > Xs;
  for( const vital::vector_2d& p : pts2d )
  {
    projs.push_back(
      cv::Point2f(
        static_cast< float >( p.x() ),
        static_cast< float >( p.y() ) ) );
  }
  for( const vital::vector_3d& X : pts3d )
  {
    Xs.push_back(
      cv::Point3f(
        static_cast< float >( X.x() ),
        static_cast< float >( X.y() ),
        static_cast< float >( X.z() ) ) );
  }

  const double reproj_error = 4;

  vital::matrix_3x3d K = cal->as_matrix();
  cv::Mat cv_K;
  cv::eigen2cv( K, cv_K );

  std::vector< double > dist_coeffs = get_ocv_dist_coeffs( cal );

  cv::Mat inliers_mat;
  cv::Mat rvec, tvec;
  bool success =
    cv::solvePnPRansac(
      Xs, projs, cv_K, dist_coeffs, rvec, tvec, false,
      this->get_max_iterations(), reproj_error,
      this->get_confidence_threshold(), inliers_mat,
      cv::SOLVEPNP_EPNP );

  double inlier_ratio = ( ( double ) inliers_mat.rows / ( double ) Xs.size() );

  if( !success || tvec.rows == 0 || rvec.rows == 0 )
  {
    LOG_DEBUG(
      this->logger(), "no PnP solution after " << this->get_max_iterations()
                                               << " iterations with confidence "
                                               << this->get_confidence_threshold()
                                               << " and best inlier ratio " <<
        inlier_ratio );

    return vital::camera_perspective_sptr();
  }

  inliers.assign( Xs.size(), 0 );

  for( int i = 0; i < inliers_mat.rows; ++i )
  {
    int idx = inliers_mat.at< int >( i );
    inliers[ idx ] = true;
  }

  auto res_cam = std::make_shared< vital::simple_camera_perspective >();
  Eigen::Vector3d rvec_eig, tvec_eig;
  cv::cv2eigen( rvec, rvec_eig );
  cv::cv2eigen( tvec, tvec_eig );

  vital::rotation_d rot( rvec_eig );
  res_cam->set_rotation( rot );
  res_cam->set_translation( tvec_eig );
  res_cam->set_intrinsics( cal );

  if( !std::isfinite( res_cam->center().x() ) )
  {
    LOG_DEBUG(
      this->logger(), "rvec " << rvec.at< double >( 0 ) << " " <<
        rvec.at< double >( 1 ) << " " << rvec.at< double >( 2 ) );
    LOG_DEBUG(
      this->logger(), "tvec " << tvec.at< double >( 0 ) << " " <<
        tvec.at< double >( 1 ) << " " << tvec.at< double >( 2 ) );
    LOG_DEBUG(
      this->logger(),
      "rotation angle " << res_cam->rotation().angle() );
    LOG_WARN(this->logger(), "non-finite camera center found" );
    return vital::camera_perspective_sptr();
  }

  return std::dynamic_pointer_cast< vital::camera_perspective >( res_cam );
}

} // end namespace ocv

} // end namespace arrows

} // end namespace kwiver
