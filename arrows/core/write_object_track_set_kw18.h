// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Interface for write_object_track_set_kw18

#ifndef KWIVER_ARROWS_WRITE_OBJECT_TRACK_SET_KW18_H
#define KWIVER_ARROWS_WRITE_OBJECT_TRACK_SET_KW18_H

#include <arrows/core/kwiver_algo_core_export.h>
#include <vital/vital_config.h>

#include <vital/algo/write_object_track_set.h>

#include <vital/algo/algorithm.h>
#include <vital/algo/algorithm.txx>

#include <memory>

namespace kwiver {

namespace arrows {

namespace core {

class KWIVER_ALGO_CORE_EXPORT write_object_track_set_kw18
  : public vital::algo::write_object_track_set
{
public:
  PLUGGABLE_IMPL(
    write_object_track_set_kw18,
    "Object track set kw18 writer.",
    PARAM_DEFAULT( delim, std::string, "delimeter", "," )
  )

  ~write_object_track_set_kw18();

  bool check_configuration( vital::config_block_sptr config ) const override;

  void write_set(
    kwiver::vital::object_track_set_sptr const& set,
    kwiver::vital::timestamp const& ts = {},
    std::string const& frame_identifier = {} ) override;

  void close() override;

private:
  void initialize() override;
  /// private implementation class
  class priv;
  KWIVER_UNIQUE_PTR( priv, d );
};

} // namespace core

} // namespace arrows

}     // end namespace

#endif // KWIVER_ARROWS_WRITE_OBJECT_TRACK_SET_KW18_H
