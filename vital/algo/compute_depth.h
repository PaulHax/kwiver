// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Header defining abstract \link kwiver::vital::algo::compute_depth
///        compute depth \endlink algorithm

#ifndef VITAL_ALGO_COMPUTE_DEPTH_H_
#define VITAL_ALGO_COMPUTE_DEPTH_H_

#include <vital/algo/algorithm.h>
#include <vital/types/bounding_box.h>
#include <vital/types/camera_perspective.h>
#include <vital/types/image_container.h>
#include <vital/types/landmark.h>
#include <vital/vital_config.h>

#include <vector>

namespace kwiver {

namespace vital {

namespace algo {

/// An abstract base class for depth map estimation
class VITAL_ALGO_EXPORT compute_depth
  : public kwiver::vital::algorithm
{
public:
  compute_depth();
  PLUGGABLE_INTERFACE( compute_depth );
  /// Compute a depth map from an image sequence
  ///
  /// Implementations of this function should not modify the underlying objects
  /// contained in the input structures. Output references should either be new
  /// instances or the same as input.
  ///
  /// \param [in] frames image sequence to compute depth with
  /// \param [in] cameras corresponding to the image sequence
  /// \param [in] depth_min minimum depth expected
  /// \param [in] depth_max maximum depth expected
  /// \param [in] reference_frame index into image sequence denoting the frame
  /// that depth is computed on
  /// \param [in] roi region of interest within reference image (can be entire
  /// image)
  /// \param [in] masks optional masks corresponding to the image sequence
  virtual kwiver::vital::image_container_sptr
  compute(
    std::vector< kwiver::vital::image_container_sptr > const& frames,
    std::vector< kwiver::vital::camera_perspective_sptr > const& cameras,
    double depth_min, double depth_max,
    unsigned int reference_frame,
    vital::bounding_box< int > const& roi,
    std::vector< kwiver::vital::image_container_sptr > const& masks =
    std::vector< kwiver::vital::image_container_sptr >( ) ) const;

  /// Compute a depth map and depth uncertainty from an image sequence
  ///
  /// Implementations of this function should not modify the underlying objects
  /// contained in the input structures. Output references should either be new
  /// instances or the same as input.
  ///
  /// \param [in] frames image sequence to compute depth with
  /// \param [in] cameras corresponding to the image sequence
  /// \param [in] depth_min minimum depth expected
  /// \param [in] depth_max maximum depth expected
  /// \param [in] reference_frame index into image sequence denoting the frame
  /// that depth is computed on
  /// \param [in] roi region of interest within reference image (can be entire
  /// image)
  /// \param [out] depth_uncertainty returns pixel-wise uncertainty
  /// \param [in] masks optional masks corresponding to the image sequence
  virtual kwiver::vital::image_container_sptr
  compute(
    std::vector< kwiver::vital::image_container_sptr > const& frames,
    std::vector< kwiver::vital::camera_perspective_sptr > const& cameras,
    double depth_min, double depth_max,
    unsigned int reference_frame,
    vital::bounding_box< int > const& roi,
    kwiver::vital::image_container_sptr& depth_uncertainty,
    std::vector< kwiver::vital::image_container_sptr > const& masks =
    std::vector< kwiver::vital::image_container_sptr >( ) ) const = 0;

  /// Typedef for the callback function signature
  typedef std::function< bool ( kwiver::vital::image_container_sptr,
                                std::string const&,
                                unsigned int,
                                kwiver::vital::image_container_sptr ) >
    callback_t;

  /// Set a callback function to report intermediate progress
  virtual void set_callback( callback_t cb );

protected:
  /// The callback function
  callback_t m_callback;
};

/// type definition for shared pointer to a compute depth algorithm
typedef std::shared_ptr< compute_depth > compute_depth_sptr;

} // namespace algo

} // namespace vital

} // namespace kwiver

#endif // VITAL_ALGO_COMPUTE_DEPTH_H_
