// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/**
 * \file
 * \brief Header for MVG camera and landmark initialization algorithm
 */

#ifndef KWIVER_ARROWS_MVG_INITIALIZE_CAMERAS_LANDMARKS_H_
#define KWIVER_ARROWS_MVG_INITIALIZE_CAMERAS_LANDMARKS_H_

#include <arrows/mvg/kwiver_algo_mvg_export.h>

#include <vital/algo/algorithm.h>
#include <vital/algo/algorithm.txx>
#include <vital/algo/bundle_adjust.h>
#include <vital/algo/estimate_canonical_transform.h>
#include <vital/algo/estimate_essential_matrix.h>
#include <vital/algo/estimate_pnp.h>
#include <vital/algo/estimate_similarity_transform.h>
#include <vital/algo/initialize_cameras_landmarks.h>
#include <vital/algo/optimize_cameras.h>
#include <vital/algo/triangulate_landmarks.h>
#include <vital/types/vector.h>

namespace kwiver {

namespace arrows {

namespace mvg {

/// A class for initialization of cameras and landmarks
class KWIVER_ALGO_MVG_EXPORT initialize_cameras_landmarks
  : public vital::algo::initialize_cameras_landmarks
{
public:
  PLUGGABLE_IMPL(
    initialize_cameras_landmarks,
    "Run SfM to estimate new cameras and landmarks "
    "using feature tracks.",
    PARAM_DEFAULT(
      verbose, bool,
      "If true, write status messages to the terminal showing "
      "debugging information", false ),

    PARAM_DEFAULT(
      force_common_intrinsics, bool,
      "If true, then all cameras will share a single set of camera "
      "intrinsic parameters", true ),

    PARAM_DEFAULT(
      frac_frames_for_init, double,
      "fraction of keyframes used in relative pose initialization", -1.0 ),

    PARAM_DEFAULT(
      min_frame_to_frame_matches, unsigned int,
      "Minimum number of frame-to-frame feature matches "
      "required to attempt reconstruction", 100 ),

    PARAM_DEFAULT(
      interim_reproj_thresh, double,
      "Threshold for rejecting landmarks based on reprojection "
      "error (in pixels) during intermediate processing steps.", 10.0 ),

    PARAM_DEFAULT(
      final_reproj_thresh, double,
      "Relative threshold for rejecting landmarks based on "
      "reprojection error relative to the median error after "
      "the final bundle adjustment.  For example, a value of 2 "
      "mean twice the median error", 2.0 ),

    PARAM_DEFAULT(
      zoom_scale_thresh, double,
      "Threshold on image scale change used to detect a camera "
      "zoom. If the resolution on target changes by more than "
      "this fraction create a new camera intrinsics model.", 0.1 ),

    PARAM_DEFAULT(
      base_camera_focal_length, double,
      "focal length of the base camera model", 1.0 ),

    PARAM(
      base_camera_principal_point, vital::vector_2d,
      "The principal point of the base camera model \"x y\".\n"
      "It is usually safe to assume this is the center of the "
      "image." ),

    PARAM_DEFAULT(
      base_camera_aspect_ratio, double,
      "the pixel aspect ratio of the base camera model", 1.5 ),

    PARAM_DEFAULT(
      base_camera_skew, double,
      "The skew factor of the base camera model.\n"
      "This is almost always zero in any real camera.", 0.0 ),

    PARAM_DEFAULT(
      max_cams_in_keyframe_init, int,
      "the maximum number of cameras to reconstruct in "
      "initialization step before switching to resectioning "
      "remaining cameras.", 20 ),

    PARAM_DEFAULT(
      metadata_init_permissive_triang_thresh, double,
      "threshold to apply to triangulation in the first "
      "permissive rounds of metadata based reconstruction "
      "initialization", 10000.0 ),

    // double ang_thresh_cur = acos(m_priv->m_thresh_triang_cos_ang) *
    // rad_to_deg;
    PARAM_DEFAULT(
      feature_angle_threshold, double,
      "feature must have this triangulation angle to keep, in degrees", 2.0 ),

    PARAM_DEFAULT(
      do_final_sfm_cleaning, bool,
      "run a final sfm solution cleanup when solution is complete", false ),
    // nested algorithm configurations
    PARAM(
      e_estimator, vital::algo::estimate_essential_matrix_sptr,
      "pointer to the nested algorithm" ),
    PARAM(
      camera_optimizer, vital::algo::optimize_cameras_sptr,
      "pointer to the nested algorithm" ),
    PARAM(
      lm_triangulator, vital::algo::triangulate_landmarks_sptr,
      "pointer to the nested algorithm" ),
    PARAM(
      bundle_adjuster, vital::algo::bundle_adjust_sptr,
      "pointer to the nested algorithm" ),
    PARAM(
      global_bundle_adjuster, vital::algo::bundle_adjust_sptr,
      "pointer to the nested algorithm" ),
    PARAM(
      estimate_pnp, vital::algo::estimate_pnp_sptr,
      "pointer to the nested algorithm" ),
    PARAM(
      canonical_estimator, vital::algo::estimate_canonical_transform_sptr,
      "pointer to the nested algorithm" ),
    PARAM(
      similarity_estimator, vital::algo::estimate_similarity_transform_sptr,
      "pointer to the nested algorithm" )
  )
  /// Destructor
  virtual ~initialize_cameras_landmarks();

  /// Check that the algorithm's currently configuration is valid
  virtual bool check_configuration( vital::config_block_sptr config ) const;

  /// Initialize the camera and landmark parameters given a set of feature
  /// tracks

  /**
   * The algorithm creates an initial estimate of any missing cameras and
   * landmarks using the available cameras, landmarks, and feature tracks.
   * It may optionally revise the estimates of exisiting cameras and landmarks.
   *
   * \param [in,out] cameras the cameras to initialize
   * \param [in,out] landmarks the landmarks to initialize
   * \param [in] tracks the feature tracks to use as constraints
   * \param [in] metadata the frame metadata to use as constraints
   */
  virtual void
  initialize(
    vital::camera_map_sptr& cameras,
    vital::landmark_map_sptr& landmarks,
    vital::feature_track_set_sptr tracks,
    vital::sfm_constraints_sptr constraints = nullptr ) const;

  /// Set a callback function to report intermediate progress
  virtual void set_callback( callback_t cb );

protected:
  void initialize() override;
  void set_configuration_internal( vital::config_block_sptr config ) override;

private:
  /// private implementation class
  class priv;
  KWIVER_UNIQUE_PTR( priv, m_priv );
};

} // end namespace mvg

} // end namespace arrows

} // end namespace kwiver

#endif
