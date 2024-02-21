// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Implementation of Pass-through convert_image

#include "convert_image_bypass.h"

#include <algorithm>

namespace kwiver {

namespace arrows {

namespace core {

// Private implementation class
class convert_image_bypass::priv
{
public:
  priv( convert_image_bypass& parent )
    : parent( parent )
  {}

  convert_image_bypass& parent;
};

// ----------------------------------------------------------------------------
void
convert_image_bypass
::initialize()
{
  KWIVER_INITIALIZE_UNIQUE_PTR( priv, d_ );
  attach_logger( "arrows.core.convert_image_bypass" );
}

// Destructor
convert_image_bypass
::~convert_image_bypass()
{}

/// Default image converter ( does nothing )
vital::image_container_sptr
convert_image_bypass
::convert( vital::image_container_sptr img ) const
{
  return img;
}

} // end namespace core

} // end namespace arrows

} // end namespace kwiver
