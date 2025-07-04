// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Header for VXL triangulate_landmarks algorithm

#ifndef KWIVER_ARROWS_VXL_TRIANGULATE_LANDMARKS_H_
#define KWIVER_ARROWS_VXL_TRIANGULATE_LANDMARKS_H_

#include <arrows/vxl/kwiver_algo_vxl_export.h>

#include <vital/algo/triangulate_landmarks.h>

namespace kwiver {

namespace arrows {

namespace vxl {

/// A class for triangulating landmarks from feature tracks and cameras using
/// VXL
class KWIVER_ALGO_VXL_EXPORT triangulate_landmarks
  : public vital::algo::triangulate_landmarks
{
public:
  PLUGGABLE_IMPL(
    triangulate_landmarks,
    "Use VXL (vpgl) to triangulate 3D landmarks from cameras and tracks." )

  /// Destructor
  virtual ~triangulate_landmarks() = default;

  /// Check that the algorithm's currently configuration is valid
  bool check_configuration( vital::config_block_sptr config ) const override;

  /// Triangulate the landmark locations given sets of cameras and feature
  /// tracks
  ///
  /// \param [in] cameras the cameras viewing the landmarks
  /// \param [in] tracks the feature tracks to use as constraints
  /// \param [in,out] landmarks the landmarks to triangulate
  ///
  /// This function only triangulates the landmarks with indicies in the
  /// landmark map and which have support in the tracks and cameras
  virtual void
  triangulate(
    vital::camera_map_sptr cameras,
    vital::feature_track_set_sptr tracks,
    vital::landmark_map_sptr& landmarks ) const;
  using vital::algo::triangulate_landmarks::triangulate;

private:
  void initialize() override;
};

} // end namespace vxl

} // end namespace arrows

} // end namespace kwiver

#endif
