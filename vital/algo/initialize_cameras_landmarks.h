// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Header defining abstract \link
/// kwiver::vital::algo::initialize_cameras_landmarks
///        bundle adjustment \endlink algorithm

#ifndef VITAL_ALGO_INITIALIZE_CAMERAS_LANDMARKS_H_
#define VITAL_ALGO_INITIALIZE_CAMERAS_LANDMARKS_H_

#include <vital/vital_config.h>

#include <vital/algo/algorithm.h>
#include <vital/types/camera_map.h>
#include <vital/types/feature_track_set.h>
#include <vital/types/landmark_map.h>
#include <vital/types/sfm_constraints.h>

#include <functional>

namespace kwiver {

namespace vital {

namespace algo {

/// An abstract base class for initialization of cameras and landmarks
class VITAL_ALGO_EXPORT initialize_cameras_landmarks
  : public kwiver::vital::algorithm
{
public:
  initialize_cameras_landmarks();

  /// Initialize the camera and landmark parameters given a set of feature
  /// tracks
  ///
  /// The algorithm creates an initial estimate of any missing cameras and
  /// landmarks using the available cameras, landmarks, and feature tracks.
  /// If the input cameras map is a NULL pointer then the algorithm should try
  /// to initialize all cameras covered by the track set.  If the input camera
  /// map exists then the algorithm should only initialize cameras on frames for
  /// which the camera is set to NULL.  Frames not in the map will not be
  /// initialized.  This allows the caller to control which subset of cameras to
  /// initialize without needing to manipulate the feature tracks.
  /// The analogous behavior is also applied to the input landmarks map to
  /// select which track IDs should be used to initialize landmarks.
  ///
  /// \note This algorithm may optionally revise the estimates of existing
  /// cameras and landmarks passed as input.
  ///
  /// \param [in,out] cameras the cameras to initialize
  /// \param [in,out] landmarks the landmarks to initialize
  /// \param [in] tracks the feature tracks to use as constraints
  /// \param [in] metadata the frame metadata to use as constraints
  virtual void
  initialize(
    kwiver::vital::camera_map_sptr& cameras,
    kwiver::vital::landmark_map_sptr& landmarks,
    kwiver::vital::feature_track_set_sptr tracks,
    kwiver::vital::sfm_constraints_sptr constraints = nullptr ) const = 0;

  /// Typedef for the callback function signature
  typedef std::function< bool ( kwiver::vital::camera_map_sptr,
                                kwiver::vital::landmark_map_sptr,
                                kwiver::vital::feature_track_set_changes_sptr ) >
    callback_t;

  /// Set a callback function to report intermediate progress
  virtual void set_callback( callback_t cb );

protected:
  /// The callback function
  callback_t m_callback;
};

/// type definition for shared pointer to an initialization algorithm
typedef std::shared_ptr< initialize_cameras_landmarks >
  initialize_cameras_landmarks_sptr;

} // namespace algo

} // namespace vital

} // namespace kwiver

#endif // VITAL_ALGO_INITIALIZE_CAMERAS_LANDMARKS_H_
