// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Header defining the core match_features_fundamental_matrix algorithm

#ifndef KWIVER_ARROWS__MATCH_FEATURES_FUNDMENTAL_MATRIX_H_
#define KWIVER_ARROWS__MATCH_FEATURES_FUNDMENTAL_MATRIX_H_

#include <arrows/core/kwiver_algo_core_export.h>

#include <vital/algo/filter_features.h>

#include <vital/algo/algorithm.h>
#include <vital/algo/algorithm.txx>

#include <vital/algo/estimate_fundamental_matrix.h>
#include <vital/algo/match_features.h>
#include <vital/config/config_block.h>

#include <vital/types/fundamental_matrix.h>
#include <vital/types/match_set.h>

namespace kwiver {

namespace arrows {

namespace core {

/// Combines a feature matcher, fundamental matrix estimation, and filtering
///
///  This is a meta-algorithm for feature matching that combines one other
/// feature
///  matcher with fundamental matrix estimation and feature filtering.
///  The algorithm applies another configurable feature matcher algorithm and
///  then applies a fundamental matrix estimation algorithm to the resulting
/// matches.
///  Outliers to the fit fundamental matrix are discarded from the set of
/// matches.
///
///  If a filter_features algorithm is provided, this will be run on the
///  input features \b before running the matcher.
class KWIVER_ALGO_CORE_EXPORT match_features_fundamental_matrix
  : public vital::algo::match_features
{
public:
  PLUGGABLE_IMPL(
    match_features_fundamental_matrix,
    "Use an estimated fundamental matrix as a geometric filter"
    " to remove outlier matches.",
    PARAM_DEFAULT(
      inlier_scale, double,
      "The acceptable error distance (in pixels) between a measured point "
      "and its epipolar line to be considered an inlier match.",
      10.0 ),
    PARAM_DEFAULT(
      min_required_inlier_count, int,
      "The minimum required inlier point count. If there are less "
      "than this many inliers, no matches will be returned.",
      0 ),
    PARAM_DEFAULT(
      min_required_inlier_percent, double,
      "The minimum required percentage of inlier points. If the "
      "percentage of points considered inliers is less than this "
      "amount, no matches will be returned.",
      0.0 ),
    PARAM_DEFAULT(
      motion_filter_percentile, double,
      "If less than 1.0, find this percentile of the motion "
      "magnitude and filter matches with motion larger than "
      "twice this value.  This helps remove outlier matches "
      "when the motion between images is small.",
      0.75 ),
    PARAM(
      matcher,
      vital::algo::match_features_sptr,
      "feature_matcher" ),
    PARAM(
      f_estimator,
      vital::algo::estimate_fundamental_matrix_sptr,
      "fundamental_matrix_estimator" )
  )

  /// Destructor
  virtual ~match_features_fundamental_matrix();

  /// Check that the algorithm's currently configuration is valid
  virtual bool check_configuration( vital::config_block_sptr config ) const;

  /// Match one set of features and corresponding descriptors to another
  ///
  /// \param [in] feat1 the first set of features to match
  /// \param [in] desc1 the descriptors corresponding to \a feat1
  /// \param [in] feat2 the second set of features to match
  /// \param [in] desc2 the descriptors corresponding to \a feat2
  /// \returns a set of matching indices from \a feat1 to \a feat2
  virtual vital::match_set_sptr
  match(
    vital::feature_set_sptr feat1, vital::descriptor_set_sptr desc1,
    vital::feature_set_sptr feat2, vital::descriptor_set_sptr desc2 ) const;

private:
  void initialize() override;
  /// private implementation class
  class priv;
  KWIVER_UNIQUE_PTR( priv, d_ );
};

} // end namespace algo

} // end namespace arrows

} // end namespace kwiver

#endif
