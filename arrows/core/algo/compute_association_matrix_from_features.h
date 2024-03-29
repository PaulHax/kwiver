// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#ifndef KWIVER_ARROWS_COMPUTE_ASSOCIATION_MATRIX_FROM_FEATURES_H_
#define KWIVER_ARROWS_COMPUTE_ASSOCIATION_MATRIX_FROM_FEATURES_H_

#include <arrows/core/kwiver_algo_core_export.h>
#include <vital/vital_config.h>

#include <vital/algo/detected_object_filter.h>

#include <vital/algo/algorithm.txx>
#include <vital/algo/compute_association_matrix.h>

namespace kwiver {

namespace arrows {

namespace core {

/// Compute an association matrix given detections and tracks
class KWIVER_ALGO_CORE_EXPORT compute_association_matrix_from_features
  : public vital::algo::compute_association_matrix
{
public:
  PLUGGABLE_IMPL(
    compute_association_matrix_from_features,
    "Populate association matrix in tracking from detector features.",
    PARAM_DEFAULT(
      max_distance,
      double,
      "Maximum allowed pixel distance for matches. Is expressed "
      "in raw pixel distance.",
      -1.0 ),
    PARAM(
      filter,
      kwiver::vital::algo::detected_object_filter_sptr,
      "filter" )
  )

  /// Destructor
  virtual ~compute_association_matrix_from_features() noexcept;

  /// Check that the algorithm's currently configuration is valid
  ///
  /// This checks solely within the provided \c config_block and not against
  /// the current state of the instance. This isn't static for inheritence
  /// reasons.
  ///
  /// \param config  The config block to check configuration of.
  ///
  /// \returns true if the configuration check passed and false if it didn't.
  virtual bool check_configuration( vital::config_block_sptr config ) const;

  /// Compute an association matrix given detections and tracks
  ///
  /// \param ts frame ID
  /// \param image contains the input image for the current frame
  /// \param tracks active track set from the last frame
  /// \param detections input detected object sets from the current frame
  /// \param matrix output matrix
  /// \param considered output detections used in matrix
  /// \return returns whether a matrix was successfully computed
  virtual bool
  compute(
    kwiver::vital::timestamp ts,
    kwiver::vital::image_container_sptr image,
    kwiver::vital::object_track_set_sptr tracks,
    kwiver::vital::detected_object_set_sptr detections,
    kwiver::vital::matrix_d& matrix,
    kwiver::vital::detected_object_set_sptr& considered ) const;

private:
  void initialize() override;
  /// private implementation class
  class priv;
  KWIVER_UNIQUE_PTR( priv, d_ );
};

} // end namespace core

} // end namespace arrows

} // end namespace kwiver

#endif
