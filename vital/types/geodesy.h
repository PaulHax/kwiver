// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief This file contains base types and structures for geodesy.

#ifndef KWIVER_VITAL_GEODESY_H_
#define KWIVER_VITAL_GEODESY_H_

#include "vector.h"
#include <vital/types/vital_types_export.h>
#include <vital/vital_config.h>

#include <map>
#include <string>

namespace kwiver {

namespace vital {

// ----------------------------------------------------------------------------
/// Well known coordinate reference systems.
///
/// This enumeration provides a set of well known coordinate reference systems
/// (CRS's). The numeric values correspond to geodetic CRS's as specified by
/// the European Petroleum Survey Group (EPSG) Spatial Reference System
/// Identifiers (SRID).
///
/// \note UTM SRID's are obtained by adding the UTM zone number to the base
///       SRID
///
/// \see https://en.wikipedia.org/wiki/Spatial_reference_system,
///      http://www.epsg.org/, https://epsg-registry.org/
namespace SRID {

constexpr int lat_lon_NAD83 = 4269;
constexpr int lat_lon_WGS84 = 4326;

constexpr int ECEF_WGS84 = 4978;

constexpr int UPS_WGS84_north = 32661;
constexpr int UPS_WGS84_south = 32761;

// Add zone number to get zoned SRID
constexpr int UTM_WGS84_north = 32600;
constexpr int UTM_WGS84_south = 32700;

// Add zone number to get zoned SRID (59N - 60N)
constexpr int UTM_NAD83_northeast = 3313;
// Add zone number to get zoned SRID (1N - 23N)
constexpr int UTM_NAD83_northwest = 26900;

} // namespace SRID

using geo_crs_description_t = std::map< std::string, std::string >;

/// Functor for implementing geodetic conversion.
class geo_conversion
{
public:
  virtual char const* id() const = 0;
  virtual geo_crs_description_t describe( int crs ) = 0;
  virtual vector_2d operator()( vector_2d const& point, int from, int to ) = 0;
  virtual vector_3d operator()( vector_3d const& point, int from, int to ) = 0;

protected:
  virtual ~geo_conversion() = default;
};

/// Get the functor used for performing geodetic conversions. \see geo_conv
VITAL_TYPES_EXPORT geo_conversion* get_geo_conv();

/// Set the functor used for performing geodetic conversions. \see geo_conv
VITAL_TYPES_EXPORT void set_geo_conv( geo_conversion* );

/// Get the description of a geodetic CRS.
VITAL_TYPES_EXPORT geo_crs_description_t geo_crs_description( int crs );

//@{
/// \brief Convert geo-coordinate.
///
/// This converts a raw geo-coordinate from one CRS to another. The numeric CRS
/// values shall correspond to geodetic CRS's as specified by the European
/// Petroleum Survey Group (EPSG) Spatial Reference System Identifiers (SRID's).
///
/// Note that the underlying values are ordered easting, northing, for
/// consistency with Euclidean convention (X, Y), and \em not northing, easting
/// as is sometimes used for geo-coordinates.
///
/// \returns The raw geo-coordinate in the requested CRS.
/// \throws std::runtime_error
///   Thrown if the conversion fails or if no conversion function has been
///   registered.
VITAL_TYPES_EXPORT vector_2d geo_conv(
  vector_2d const& point, int from,
  int to );
VITAL_TYPES_EXPORT vector_3d geo_conv(
  vector_3d const& point, int from,
  int to );
//@}

/// UTM/UPS zone specification.
struct utm_ups_zone_t
{
  int number; /// Zone number; 1-60 is UTM, 0 is UPS.
  bool north; /// Indicates if zone if north or south.
};

//@{
/// \brief Determine UTM/UPS zone of lat/lon geo-coordinate.
///
/// This determines the appropriate greater UTM or UPS zone given an input
/// coordinate in a latitude/longitude coordinate system. "Greater zone" here
/// means that UTM zones are distinguished only by north/south; the irregular
/// grid zones in northern Europe are not considered.
///
/// The resulting zone will be appropriate for the input datum; for example,
/// input in NAD83 lat/lon will produce a result suitable for representing in
/// NAD83 UTM. The user is responsible for ensuring that the input coordinate is
/// in a lat/lon system.
///
/// Note that the coordinate values are assumed to be in degrees (not radians),
/// in the order easting (longitude), northing (latitude), for consistency with
/// geo_point. Out of range longitude values are normalized.
///
/// \returns The UTM/UPS zone information.
/// \throws std::range_error
///   Thrown if the latitude (northing) value is outside of the range
///   <code>[-90, 90]</code>.
VITAL_TYPES_EXPORT utm_ups_zone_t utm_ups_zone( double lon, double lat );
VITAL_TYPES_EXPORT utm_ups_zone_t utm_ups_zone( vector_2d const& lon_lat );
VITAL_TYPES_EXPORT utm_ups_zone_t utm_ups_zone( vector_3d const& lon_lat_alt );
//@}

} // namespace vital

}   // end namespace

#endif
