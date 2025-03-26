// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <vital/types/local_tangent_space.h>

#include <vital/types/geodesy.h>

#include <iomanip>
#include <iostream>
#include <vital/math_constants.h>

namespace kwiver {

namespace vital {

// ----------------------------------------------------------------------------
local_tangent_space
::local_tangent_space( geo_point const& origin )
  : m_origin{ origin },
    m_axes{}
{
  auto const ecef = m_origin.location( SRID::ECEF_WGS84 );

  // Check for being on the polar axis and return standardized coordinates
  constexpr double epsilon = 1e-6;
  if( std::abs( ecef[ 0 ] ) < epsilon && std::abs( ecef[ 1 ] ) < epsilon )
  {
    auto const sign = ecef[ 2 ] < 0.0 ? -1.0 : 1.0;
    m_axes <<
      1.0, 0.0, 0.0,
      0.0, sign, 0.0,
      0.0, 0.0, sign;
    return;
  }

  auto const lon_lat = m_origin.location( SRID::lat_lon_WGS84 );

  // Compute tangents using trigonometry
  auto const sin_lon = std::sin( lon_lat[ 0 ] * deg_to_rad );
  auto const cos_lon = std::cos( lon_lat[ 0 ] * deg_to_rad );
  auto const sin_lat = std::sin( lon_lat[ 1 ] * deg_to_rad );
  auto const cos_lat = std::cos( lon_lat[ 1 ] * deg_to_rad );

  m_axes <<
    -sin_lon, cos_lon, 0.0, // East
    -sin_lat * cos_lon, -sin_lat * sin_lon, cos_lat, // North
    cos_lon* cos_lat, sin_lon* cos_lat, sin_lat;   // Up
}

// ----------------------------------------------------------------------------
geo_point const&
local_tangent_space
::origin() const
{
  return m_origin;
}

// ----------------------------------------------------------------------------
vector_3d
local_tangent_space
::to_local( geo_point const& global_point ) const
{
  vector_3d point = global_point.location( SRID::ECEF_WGS84 );
  point -= m_origin.location( SRID::ECEF_WGS84 );
  return m_axes * point;
}

// ----------------------------------------------------------------------------
geo_point
local_tangent_space
::to_global( vector_3d const& local_point ) const
{
  vector_3d point = m_axes.transpose() * local_point;
  point += m_origin.location( SRID::ECEF_WGS84 );
  return { point, SRID::ECEF_WGS84 };
}

} // namespace vital

} // namespace kwiver
