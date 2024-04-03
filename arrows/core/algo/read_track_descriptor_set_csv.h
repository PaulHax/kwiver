// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Interface for read_track_descriptor_set_csv

#ifndef KWIVER_ARROWS_TRACK_DESCRIPTOR_SET_OUTPUT_CSV_H
#define KWIVER_ARROWS_TRACK_DESCRIPTOR_SET_OUTPUT_CSV_H

#include <arrows/core/kwiver_algo_core_export.h>

#include <vital/algo/algorithm.txx>
#include <vital/algo/read_track_descriptor_set.h>

namespace kwiver {

namespace arrows {

namespace core {

class KWIVER_ALGO_CORE_EXPORT read_track_descriptor_set_csv
  : public vital::algo::read_track_descriptor_set
{
public:
  PLUGGABLE_IMPL(
    read_track_descriptor_set_csv,
    "Track descriptor csv reader",
    PARAM_DEFAULT( batch_load, bool, "batch_load", true ),
    PARAM_DEFAULT( read_raw_descriptor, bool, "read_raw_descriptor", true ),
  )

  virtual ~read_track_descriptor_set_csv();

  virtual bool check_configuration( vital::config_block_sptr config ) const;

  virtual bool read_set( kwiver::vital::track_descriptor_set_sptr& set );

private:
  void initialize() override;

  class priv;
  KWIVER_UNIQUE_PTR( priv, d );
};

} // namespace core

} // namespace arrows

}     // end namespace

#endif // KWIVER_ARROWS_TRACK_DESCRIPTOR_SET_OUTPUT_CSV_H
