// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Header for mesh uv unwrapping

#ifndef KWIVER_ARROWS_CORE_UV_UNWRAP_MESH_H
#define KWIVER_ARROWS_CORE_UV_UNWRAP_MESH_H

#include <arrows/core/kwiver_algo_core_export.h>

#include <vital/algo/uv_unwrap_mesh.h>
#include <vital/types/mesh.h>
#include <vital/vital_config.h>

#include <vital/algo/algorithm.txx>

#include "vital/plugin_management/pluggable_macro_magic.h"

namespace kwiver {

namespace arrows {

namespace core {

/// A class for unwrapping a mesh and generating texture coordinates
class KWIVER_ALGO_CORE_EXPORT uv_unwrap_mesh
  : public vital::algo::uv_unwrap_mesh
{
public:
  PLUGGABLE_IMPL(
    uv_unwrap_mesh,
    "A class for unwrapping a mesh and generating texture coordinates. ",
    PARAM_DEFAULT(
      spacing, double,
      "Spacing between triangles. It is a percentage of the texture size "
      "and should be relatively small (default is 0.005).",
      0.005 )
  )

  /// Destructor
  virtual ~uv_unwrap_mesh();

  /// Check configuration
  bool check_configuration( vital::config_block_sptr config ) const override;

  /// Unwrap a mesh and generate texture coordinate
  ///
  /// \param mesh [in/out]
  void unwrap( kwiver::vital::mesh_sptr mesh ) const override;

private:
  void initialize() override;
  /// private implementation class
  class priv;
  KWIVER_UNIQUE_PTR( priv, d_ );
};

} // namespace core

} // namespace arrows

} // namespace kwiver

#endif // KWIVER_ARROWS_CORE_UV_UNWRAP_MESH_H
