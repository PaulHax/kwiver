// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#ifndef VITAL_TYPES_LOCAL_TANGENT_SPACE_H_
#define VITAL_TYPES_LOCAL_TANGENT_SPACE_H_

#include <vital/types/geo_point.h>
#include <vital/types/vital_types_export.h>

namespace kwiver {

namespace vital {

// ----------------------------------------------------------------------------
/// Local coordinate system tangent to the earth.
///
/// This class comes with the following guarantees:
///   1. The coordinate system is always cartesian; distances do not curve
///      around the Earth.
///   2. The coordinate system is always right-handed.
///   3. The coordinate system is always expressed in meters.
///   4. If the origin is not at the center of the earth, The X-Y plane is
///      tangent to the earth's surface at the origin and the Z axis points up.
///   5. If the origin is not along the polar axis, the X axis points east and
///      the Y axis points north.
struct VITAL_TYPES_EXPORT local_tangent_space
{
public:
  explicit local_tangent_space( geo_point const& origin );

  geo_point const& origin() const;

  vector_3d to_local( geo_point const& global_point ) const;
  geo_point to_global( vector_3d const& local_point ) const;

private:
  geo_point const m_origin;
  Eigen::Matrix3d m_axes;
};

} // namespace vital

} // namespace kwiver

#endif
