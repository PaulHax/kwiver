// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief uv_unwrap_mesh algorithm instantiation

#include "uv_unwrap_mesh.h"

namespace kwiver {

namespace vital {

namespace algo {

uv_unwrap_mesh
::uv_unwrap_mesh()
{
  attach_logger( "algo.uv_unwrap_mesh" );
}

// Destructor
uv_unwrap_mesh
::~uv_unwrap_mesh()
{
}

} // namespace algo

} // namespace vital

} // namespace kwiver
