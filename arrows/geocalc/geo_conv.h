// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#ifndef KWIVER_ARROWS_GEOCALC_GEO_CONV_H_
#define KWIVER_ARROWS_GEOCALC_GEO_CONV_H_

#include <arrows/geocalc/kwiver_algo_geocalc_export.h>

#include <vital/types/geodesy.h>

namespace kwiver {

namespace arrows {

namespace geocalc {

// ----------------------------------------------------------------------------
class KWIVER_ALGO_GEOCALC_EXPORT geo_conversion
  : public vital::geo_conversion
{
public:
  static geo_conversion& instance();

  char const* id() const override;

  vital::geo_crs_description_t describe( int crs ) override;

  vital::vector_2d operator()(
    vital::vector_2d const& point, int from, int to ) override;

  vital::vector_3d operator()(
    vital::vector_3d const& point, int from, int to ) override;
};

} // namespace geocalc

} // namespace arrows

} // namespace kwiver

#endif
