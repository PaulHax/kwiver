// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/**
 * \file
 * \brief Header for MVG triangulate_landmarks algorithm
 */

#ifndef KWIVER_ARROWS_MVG_TRIANGULATE_LANDMARKS_H_
#define KWIVER_ARROWS_MVG_TRIANGULATE_LANDMARKS_H_

#include <arrows/mvg/kwiver_algo_mvg_export.h>

#include <vital/algo/algorithm.h>
#include <vital/algo/algorithm.txx>
#include <vital/algo/triangulate_landmarks.h>

namespace kwiver {
namespace arrows {
namespace mvg {

/// A class for triangulating landmarks from feature tracks and cameras using Eigen
class KWIVER_ALGO_MVG_EXPORT triangulate_landmarks
: public vital::algo::triangulate_landmarks
{
public:
  PLUGGABLE_IMPL( triangulate_landmarks,
               "Triangulate landmarks from tracks and cameras"
               " using a simple least squares solver.",
    PARAM_DEFAULT(homogeneous, bool,
                    "Use the homogeneous method for triangulating points. "
                    "The homogeneous method can triangulate points at or near "
                    "infinity and discard them.", false),

    PARAM_DEFAULT(ransac, bool,
                    "Use RANSAC in triangulating the points", true),

    PARAM_DEFAULT(min_angle_deg, float,
                    "minimum angle required to triangulate a point.", 1.0f),

    PARAM_DEFAULT(inlier_threshold_pixels, float,
                   "reprojection error threshold in pixels.", 2.0f),

    PARAM_DEFAULT(frac_track_inliers_to_keep_triangulated_point, float,
                    "fraction of measurements in track that must be inliers to "
                    "keep the triangulated point", 0.5f),

    PARAM_DEFAULT(max_ransac_samples, int,
                    "maximum number of samples to take in RANSAC triangulation", 20),

    PARAM_DEFAULT(conf_thresh, double,
                    "RANSAC sampling terminates when this confidences in the "
                    "solution is reached.", 0.99)
                )

  /// Destructor
  virtual ~triangulate_landmarks();

  /// Check that the algorithm's currently configuration is valid
  virtual bool check_configuration(vital::config_block_sptr config) const;

  /// Triangulate the landmark locations given sets of cameras and feature tracks
  /**
   * \param [in] cameras the cameras viewing the landmarks
   * \param [in] tracks the feature tracks to use as constraints
   * \param [in,out] landmarks the landmarks to triangulate
   *
   * This function only triangulates the landmarks with indicies in the
   * landmark map and which have support in the tracks and cameras.  Note:
   * triangulate modifies the inlier/outlier flags in tracks. It also sets
   * the cosine of the maximum observation angle and number of observations
   * in the landmarks.
   */
  virtual void
  triangulate(vital::camera_map_sptr cameras,
              vital::feature_track_set_sptr tracks,
              vital::landmark_map_sptr& landmarks) const;

  /// Triangulate the landmark locations given sets of cameras and feature tracks
  /**
  * \param [in] cameras the cameras viewing the landmarks
  * \param [in] tracks the feature tracks to use as constraints in a map
  * \param [in,out] landmarks the landmarks to triangulate
  *
  * This function only triangulates the landmarks with indicies in the
  * landmark map and which have support in the tracks and cameras.  Note:
  * triangulate modifies the inlier/outlier flags in tracks. It also sets
  * the cosine of the maximum observation angle and number of observations
  * in the landmarks.
  */
  virtual void
  triangulate(vital::camera_map_sptr cameras,
              vital::track_map_t tracks,
              vital::landmark_map_sptr& landmarks) const;

protected:
  void initialize() override;

private:
  /// private implementation class
  class priv;
  KWIVER_UNIQUE_PTR(priv,d_);
};

typedef std::shared_ptr<triangulate_landmarks> triangulate_landmarks_sptr;

} // end namespace mvg
} // end namespace arrows
} // end namespace kwiver

#endif
