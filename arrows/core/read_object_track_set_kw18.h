// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Interface for read_object_track_set_kw18

#ifndef KWIVER_ARROWS_READ_OBJECT_TRACK_SET_KW18_H
#define KWIVER_ARROWS_READ_OBJECT_TRACK_SET_KW18_H

#include <arrows/core/kwiver_algo_core_export.h>
#include <vital/vital_config.h>

#include <vital/algo/algorithm.txx>
#include <vital/algo/read_object_track_set.h>

#include <memory>

namespace kwiver {

namespace arrows {

namespace core {

class KWIVER_ALGO_CORE_EXPORT read_object_track_set_kw18
  : public vital::algo::read_object_track_set
{
public:
  PLUGGABLE_IMPL(
    read_object_track_set_kw18,
    "Object track set kw18 reader.",
    PARAM_DEFAULT( delim, std::string, "delimeter", " " ),
    PARAM_DEFAULT( batch_load, bool, "batch_load", true ),
  )

  virtual ~read_object_track_set_kw18();

  virtual bool check_configuration( vital::config_block_sptr config ) const;

  virtual bool read_set( kwiver::vital::object_track_set_sptr& set );

private:
  void initialize() override;

  /// private implementation class
  class priv;
  KWIVER_UNIQUE_PTR( priv, d );
};

} // namespace core

} // namespace arrows

}     // end namespace

#endif // KWIVER_ARROWS_READ_OBJECT_TRACK_SET_KW18_H
