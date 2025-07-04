// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief VXL homography estimation algorithm implementation

#include "estimate_homography.h"

#include <rrel/rrel_homography2d_est.h>
#include <rrel/rrel_irls.h>
#include <rrel/rrel_ran_sam_search.h>
#include <rrel/rrel_trunc_quad_obj.h>

#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_3x3.h>

using namespace kwiver::vital;

namespace kwiver {

namespace arrows {

namespace vxl {

// ----------------------------------------------------------------------------
void
estimate_homography
::initialize()
{
  attach_logger( "arrows.vxl.estimate_homography" );
}

// ----------------------------------------------------------------------------
/// Estimate a homography matrix from corresponding points
homography_sptr
estimate_homography
::estimate(
  const std::vector< vector_2d >& pts1,
  const std::vector< vector_2d >& pts2,
  std::vector< bool >& inliers,
  double inlier_scale ) const
{
  if( pts1.size() < 4 || pts2.size() < 4 )
  {
    vital::logger_handle_t logger( vital::get_logger(
      "arrows.vxl.estimate_homography" ) );
    LOG_ERROR(logger, "Not enough points to estimate a homography" );
    return homography_sptr();
  }

  std::vector< vnl_vector< double > > from_pts, to_pts;
  for( const vector_2d& v : pts1 )
  {
    from_pts.push_back(
      vnl_vector< double >(
        vnl_double_3(
          v.x(), v.y(),
          1.0 ) ) );
  }
  for( const vector_2d& v : pts2 )
  {
    to_pts.push_back(
      vnl_vector< double >(
        vnl_double_3(
          v.x(), v.y(),
          1.0 ) ) );
  }

  // Step 1: estimate the homography using sampling.  This will allow
  // a good rejection of outliers.
  rrel_homography2d_est hg( from_pts, to_pts );
  hg.set_prior_scale( inlier_scale );

  rrel_trunc_quad_obj msac;
  // TODO expose these parameters
  rrel_ran_sam_search ransam( 42 );
  ransam.set_sampling_params( 0.80 );
  ransam.set_trace_level( 0 );

  bool result = ransam.estimate( &hg, &msac );

  std::vector< double > residuals = ransam.residuals();

  if( !result )
  {
    std::cerr << "MSAC failed!!" << std::endl;
    return homography_sptr();
  }

  // Step 2: refine the estimate using weighted least squares.  This
  // will allow us to estimate a homography that does not exactly
  // fit 4 points, which will be a better estimate.  The ransam
  // estimate from step 2 would have gotten us close enough to the
  // correct solution for IRLS to work.
  rrel_irls irls;
  irls.set_no_scale_est();
  irls.initialize_scale( inlier_scale );
  irls.initialize_params( ransam.params() );

  bool result2 = irls.estimate( &hg, &msac );

  vnl_double_3x3 m;
  if( !result2 )
  {
    // if the IRLS fails, fall back to the ransam estimate.
    std::cerr << "IRLS failed" << std::endl;
    hg.params_to_homog( ransam.params(), m.as_ref().non_const() );
  }
  else
  {
    hg.params_to_homog( irls.params(), m.as_ref().non_const() );
    hg.compute_residuals( irls.params(), residuals );
  }

  inliers.clear();
  for( const double& r : residuals )
  {
    inliers.push_back( r < inlier_scale );
  }

  Eigen::Matrix< double, 3, 3 > r_mat =
    Eigen::Matrix< double, 3, 3 >( m.data_block() ).transpose();
  return homography_sptr( new homography_< double >( r_mat ) );
}

} // end namespace vxl

} // end namespace arrows

} // end namespace kwiver
