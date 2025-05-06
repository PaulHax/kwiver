// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// Interface to the KLV lengthy template class.

#ifndef KWIVER_ARROWS_KLV_KLV_LENGTHY_H_
#define KWIVER_ARROWS_KLV_KLV_LENGTHY_H_

#include <arrows/klv/klv_util.h>
#include <arrows/klv/kwiver_algo_klv_export.h>

#include <ostream>

#include <cstddef>

namespace kwiver {

namespace arrows {

namespace klv {

// ----------------------------------------------------------------------------
/// A container holding some other type plus a byte count.
///
/// This is used when a data format can encode some value with a user-specified
/// number of bytes, perhaps to control encoded precision. Recording the byte
/// count is important in that situation to avoid gaining or losing precision
/// when re-encoding. This container should not be used when the number of bytes
/// is fully determined by the value of the wrapped data type and/or the overall
/// context of the data format.
template < class T >
struct KWIVER_ALGO_KLV_EXPORT klv_lengthy
{
  using value_type = T;

  klv_lengthy( T const& value );
  klv_lengthy( T&& value );
  klv_lengthy( T const& value, size_t length );
  klv_lengthy( T&& value, size_t length );

  T value;
  size_t length;
};

// ----------------------------------------------------------------------------
template < class T >
KWIVER_ALGO_KLV_EXPORT
std::ostream&
operator<<( std::ostream& os, klv_lengthy< T > const& value );

// ----------------------------------------------------------------------------
DECLARE_TEMPLATE_CMP( klv_lengthy< T > )

} // namespace klv

} // namespace arrows

} // namespace kwiver

#endif
