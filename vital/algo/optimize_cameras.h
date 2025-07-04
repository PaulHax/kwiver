// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Header defining abstract \link kwiver::vital::algo::optimize_cameras
/// camera
///        optimization \endlink algorithm

#ifndef VITAL_ALGO_OPTIMIZE_CAMERAS_H_
#define VITAL_ALGO_OPTIMIZE_CAMERAS_H_

#include <vital/vital_config.h>

#include <vital/algo/algorithm.h>
#include <vital/types/camera_map.h>
#include <vital/types/camera_perspective.h>
#include <vital/types/feature_track_set.h>
#include <vital/types/landmark_map.h>
#include <vital/types/sfm_constraints.h>

namespace kwiver {

namespace vital {

namespace algo {

/// Abstract algorithm definition base for optimizing cameras
class VITAL_ALGO_EXPORT optimize_cameras
  : public kwiver::vital::algorithm
{
public:
  optimize_cameras();
  PLUGGABLE_INTERFACE( optimize_cameras );
  /// Optimize camera parameters given sets of landmarks and feature tracks
  ///
  /// We only optimize cameras that have associating tracks and landmarks in
  /// the given maps.  The default implementation collects the corresponding
  /// features and landmarks for each camera and calls the single camera
  /// optimize function.
  ///
  /// \throws invalid_value When one or more of the given pointer is Null.
  ///
  /// \param[in,out] cameras   Cameras to optimize.
  /// \param[in]     tracks    The feature tracks to use as constraints.
  /// \param[in]     landmarks The landmarks the cameras are viewing.
  /// \param[in]     metadata  The optional metadata to constrain the
  ///                          optimization.
  virtual void
  optimize(
    kwiver::vital::camera_map_sptr& cameras,
    kwiver::vital::feature_track_set_sptr tracks,
    kwiver::vital::landmark_map_sptr landmarks,
    kwiver::vital::sfm_constraints_sptr constraints = nullptr ) const;

  /// Optimize a single camera given corresponding features and landmarks
  ///
  /// This function assumes that 2D features viewed by this camera have
  /// already been put into correspondence with 3D landmarks by aligning
  /// them into two parallel vectors
  ///
  /// \param[in,out] camera    The camera to optimize.
  /// \param[in]     features  The vector of features observed by \p camera
  ///                          to use as constraints.
  /// \param[in]     landmarks The vector of landmarks corresponding to
  ///                          \p features.
  /// \param[in]     metadata  The optional metadata to constrain the
  ///                          optimization.
  virtual void
  optimize(
    kwiver::vital::camera_perspective_sptr& camera,
    const std::vector< kwiver::vital::feature_sptr >& features,
    const std::vector< kwiver::vital::landmark_sptr >& landmarks,
    kwiver::vital::sfm_constraints_sptr constraints = nullptr ) const = 0;
};

/// Type definition for shared pointer to an optimize cameras algorithm
typedef std::shared_ptr< optimize_cameras > optimize_cameras_sptr;

} // namespace algo

} // namespace vital

} // namespace kwiver

#endif // VITAL_ALGO_OPTIMIZE_CAMERAS_H_
