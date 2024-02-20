// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Header for metadata derivation utility functions.

#ifndef KWIVER_ARROWS_CORE_DERIVE_METADATA_H_
#define KWIVER_ARROWS_CORE_DERIVE_METADATA_H_

#include <arrows/core/kwiver_algo_core_export.h>

#include <vital/algo/algorithm.txx>

#include <vital/algo/metadata_filter.h>

namespace kwiver {

namespace arrows {

namespace core {

class KWIVER_ALGO_CORE_EXPORT derive_metadata
  : public vital::algo::metadata_filter
{
public:
  PLUGGABLE_IMPL(
    derive_metadata,
    "Fills in metadata values which can be calculated "
    "from other metadata."
  )

  virtual ~derive_metadata();

  bool check_configuration( vital::config_block_sptr config ) const override;

  /// Fills in metadata values which can be calculated from other metadata.
  ///
  /// \param input_metadata Input vector of metadata packets.
  /// \param input_image Associated video image.
  ///
  /// \returns An amended metadata vector.
  kwiver::vital::metadata_vector filter(
    kwiver::vital::metadata_vector const& input_metadata,
    kwiver::vital::image_container_scptr const& input_image ) override;

private:
  void initialize() override;
  /// private implementation class
  class priv;
  KWIVER_UNIQUE_PTR( priv, d_ );
};

} // namespace core

} // namespace arrows

} // namespace kwiver

#endif
