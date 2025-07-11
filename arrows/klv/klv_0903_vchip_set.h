// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// Interface to the KLV 0903 VChip local set parser.

#ifndef KWIVER_ARROWS_KLV_KLV_0903_VCHIP_SET_H
#define KWIVER_ARROWS_KLV_KLV_0903_VCHIP_SET_H

#include <arrows/klv/kwiver_algo_klv_export.h>

#include <arrows/klv/klv_packet.h>
#include <arrows/klv/klv_series.h>
#include <arrows/klv/klv_set.h>

#include <ostream>

namespace kwiver {

namespace arrows {

namespace klv {

// ----------------------------------------------------------------------------
/// Tag values for the ST0903 vChip local set.
enum klv_0903_vchip_set_tag : klv_lds_key
{
  KLV_0903_VCHIP_UNKNOWN        = 0,
  KLV_0903_VCHIP_IMAGE_TYPE     = 1,
  KLV_0903_VCHIP_IMAGE_IRI      = 2,
  KLV_0903_VCHIP_EMBEDDED_IMAGE = 3,
};

// ----------------------------------------------------------------------------
KWIVER_ALGO_KLV_EXPORT
std::ostream&
operator<<( std::ostream& os, klv_0903_vchip_set_tag tag );

// ----------------------------------------------------------------------------
/// Returns a lookup object for the traits of the ST0903 vChip local set tags.
KWIVER_ALGO_KLV_EXPORT
klv_tag_traits_lookup const&
klv_0903_vchip_set_traits_lookup();

// ----------------------------------------------------------------------------
/// Interprets data as a ST0903 vChip local set.
class KWIVER_ALGO_KLV_EXPORT klv_0903_vchip_local_set_format
  : public klv_local_set_format
{
public:
  klv_0903_vchip_local_set_format();

  std::string
  description_() const override;
};

// ----------------------------------------------------------------------------
/// Interprets data as a ST0903 vChip series.
using klv_0903_vchip_series_format =
  klv_series_format< klv_0903_vchip_local_set_format >;

} // namespace klv

} // namespace arrows

} // namespace kwiver

#endif
