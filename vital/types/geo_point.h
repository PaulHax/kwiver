// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief This file contains the interface to a geo point.

#ifndef KWIVER_VITAL_GEO_POINT_H_
#define KWIVER_VITAL_GEO_POINT_H_

#include <vital/types/vector.h>
#include <vital/types/vital_types_export.h>
#include <vital/vital_config.h>

#include <unordered_map>

namespace kwiver {

namespace vital {

// ----------------------------------------------------------------------------
/// Geo-coordinate.
///
/// This class represents a geolocated point. The point is created by specifying
/// a raw location and a CRS. The original location and original CRS may be
/// directly accessed, or the location in a specific CRS may be requested.
/// Requests for a specific CRS are cached, so that CRS conversion does not need
/// to be performed every time.
///
/// The CRS values shall correspond to geodetic CRS's as specified by the
/// European Petroleum Survey Group (EPSG) Spatial Reference System Identifiers
/// (SRID's). Some well known values are defined by kwiver::vital::SRID.
///
/// Note that the underlying values are ordered easting, northing, for
/// consistency with Euclidean convention (X, Y), and \em not northing, easting
/// as is sometimes used for geo-coordinates.
///
/// \see https://en.wikipedia.org/wiki/Spatial_reference_system,
///      http://www.epsg.org/, https://epsg-registry.org/
class VITAL_TYPES_EXPORT geo_point
{
public:
  using geo_3d_point_t = kwiver::vital::vector_3d;
  using geo_2d_point_t = kwiver::vital::vector_2d;

  geo_point();
  geo_point( geo_2d_point_t const&, int crs );
  geo_point( geo_3d_point_t const&, int crs );

  virtual ~geo_point() = default;

  /// \brief Accessor for location in original CRS.
  ///
  /// \returns The location in the CRS that was used to set the location.
  /// \throws std::out_of_range Thrown if no location has been set.
  ///
  /// \see crs()
  geo_3d_point_t location() const;

  /// \brief Accessor for original CRS.
  ///
  /// \returns The CRS used to set the location.
  ///
  /// \see location()
  int crs() const;

  /// \brief Accessor for the location.
  ///
  /// \returns The location in the requested CRS.
  /// \throws std::runtime_error if the conversion fails.
  geo_3d_point_t location( int crs ) const;

  //@{
  /// \brief Set location.
  ///
  /// This sets the geo-coordinate to the specified location, which is defined
  /// by the raw location and specified CRS.
  void set_location( geo_2d_point_t const&, int crs );
  void set_location( geo_3d_point_t const&, int crs );
  //@}

  /// \brief Test if point has a specified location.
  ///
  /// This method checks the object to see if any location data has been set.
  ///
  /// \returns \c true if object is default constructed.
  bool is_empty() const;

protected:
  int m_original_crs;
  mutable std::unordered_map< int, geo_3d_point_t > m_loc;
};

VITAL_TYPES_EXPORT::std::ostream& operator<<(
  ::std::ostream& str,
  geo_point const& obj );

VITAL_TYPES_EXPORT
bool operator==( geo_point const& lhs, geo_point const& rhs );

VITAL_TYPES_EXPORT
bool operator!=( geo_point const& lhs, geo_point const& rhs );

} // namespace vital

}   // end namespace

#endif
