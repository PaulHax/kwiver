// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#ifndef KWIVER_ARROWS_GEOCALC_PROJECTION_H_
#define KWIVER_ARROWS_GEOCALC_PROJECTION_H_

#include <arrows/geocalc/kwiver_algo_geocalc_export.h>

#include <vital/types/vector.h>

#include <optional>

namespace kwiver {

namespace arrows {

namespace geocalc {

// ----------------------------------------------------------------------------
/// Return the nearest point along a ray from \p point along \p vector that
/// intersects the ellipsoid at \p altitude, if such a point exists.
///
/// \param point Starting position of ray, in ECEF coordinates.
/// \param vector Direction of ray, using ECEF reference frame.
/// \param crs ECEF coordinate system used.
/// \param altitude How far above or below the ellipsoid to cast the ray.
/// \param throw_on_interior_point
///   Whether to throw a \c std::runtime_error if \p point is inside the
///   ellipsoid. This is \c true by default to prevent raycasts pointing "down"
///   from slightly below \p altitude from producing points on the other side of
///   the Earth, which is likely not intended. Set \p throw_on_interior_point to
///   \c false to opt in to this behavior.
///
/// \return
///   Nearest intersecting point along the ray in ECEF coordinates, or
///   \c std::nullopt if the ray does not intersect the ellipsoid.
KWIVER_ALGO_GEOCALC_EXPORT
std::optional< vital::vector_3d >
raycast_ecef_to_ellipsoid(
  vital::vector_3d const& point,
  vital::vector_3d const& vector,
  int crs,
  double altitude = 0.0,
  bool throw_on_interior_point = true );

} // namespace geocalc

} // namespace arrows

} // namespace kwiver

#endif
