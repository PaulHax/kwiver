// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// Operations to calculate closest points and ray intersections
/// to triangles and meshes.

#ifndef KWIVER_ARROWS_CORE_MESH_INTERSECT_H
#define KWIVER_ARROWS_CORE_MESH_INTERSECT_H

#include <arrows/core/kwiver_algo_core_export.h>

#include <vital/types/mesh.h>
#include <vital/types/point.h>

namespace kwiver {

namespace arrows {

namespace core {

/// Intersect the ray from point to a triangle
///
/// Intersect the ray from point p with direction d and the triangle
/// defined by a,b,c. Returns the distance to the mesh from the point
/// and the barycentric coordinates of the intersection on the triangle.
///
/// \param [in]   p     point that is the start of the ray
/// \param [in]   d     direction of the ray
/// \param [in]   a     corner point of triangle
/// \param [in]   b     corner point of triangle
/// \param [in]   c     corner point of triangle
/// \param [out]  dist  the distance to the triangle
/// \param [out]  u     barycentric coordinate of the intersection
/// \param [out]  v     barycentric coordinate of the intersection
/// \returns      true  if intersection occurs
/// Barycentric coordinates are u and v such that (1-u-v)*a + u*b + v*c =
/// p+dist*d
KWIVER_ALGO_CORE_EXPORT
bool
mesh_intersect_triangle(
  const vital::point_3d& p,
  const vital::vector_3d& d,
  const vital::point_3d& a,
  const vital::point_3d& b,
  const vital::point_3d& c,
  double& dist,
  double& u, double& v );

/// Intersect a ray from point to a triangle
///
/// Intersect the ray from point p with direction d and the triangle defined
/// by a,b,c. The un-normalized normal vector (b-a)x(c-a) is precomputed and
/// also passed in. Returns the distance to the mesh from the point
/// and the barycentric coordinates of the intersection on the triangle.
///
/// \param [in]   p     point that is the start of the ray
/// \param [in]   d     direction of the ray
/// \param [in]   a     corner point of triangle
/// \param [in]   b     corner point of triangle
/// \param [in]   c     corner point of triangle
/// \param [in]   n     pre-computed normal for triangle
/// \param [out]  dist  the distance to the triangle
/// \param [out]  u     barycentric coordinate of the intersection
/// \param [out]  v     barycentric coordinate of the intersection
/// \returns      true  if intersection occurs
/// Barycentric coordinates are u and v such that (1-u-v)*a + u*b + v*c =
/// p+dist*d
KWIVER_ALGO_CORE_EXPORT
bool
mesh_intersect_triangle(
  const vital::point_3d& p,
  const vital::vector_3d& d,
  const vital::point_3d& a,
  const vital::point_3d& b,
  const vital::point_3d& c,
  const vital::vector_3d& n,
  double& dist,
  double& u, double& v );

/// Intersect the ray from point to a triangle and check if the distance is
/// smaller
///
/// Intersect the ray from point p with direction d and the triangle defined
/// by a,b,c. The un-normalized normal vector (b-a)x(c-a) is precomputed and
/// also passed in. Returns the distance to the mesh from the point
/// and the barycentric coordinates of the intersection on the triangle if
/// the distance is smaller.
///
/// \param [in]   p     point that is the start of the ray
/// \param [in]   d     direction of the ray
/// \param [in]   a     corner point of triangle
/// \param [in]   b     corner point of triangle
/// \param [in]   c     corner point of triangle
/// \param [in]   n     pre-computed normal for triangle
/// \param [out]  dist  the distance to the triangle
/// \param [out]  u     barycentric coordinate of the intersection
/// \param [out]  v     barycentric coordinate of the intersection
/// \returns      true  if intersection occurs and the new dist is less than the
///                     old distance (but > 0)
/// Barycentric coordinates are u and v such that (1-u-v)*a + u*b + v*c =
/// p+dist*d
KWIVER_ALGO_CORE_EXPORT
bool
mesh_intersect_triangle_min_dist(
  const vital::point_3d& p,
  const vital::vector_3d& d,
  const vital::point_3d& a,
  const vital::point_3d& b,
  const vital::point_3d& c,
  const vital::vector_3d& n,
  double& dist,
  double& u, double& v );

/// Find the closest point on the triangle to a reference point
///
/// Find the closest point on the triangle a,b,c to point p. The un-normalized
/// normal vector (b-a)x(c-a) is precomputed and also passed in. Returns the
/// distance to the mesh from the point and the barycentric coordinates on
/// the triangle of the intersection.
///
/// \param [in]   p     reference point to get closest distance to
/// \param [in]   a     corner point of triangle
/// \param [in]   b     corner point of triangle
/// \param [in]   c     corner point of triangle
/// \param [in]   n     pre-computed normal for triangle
/// \param [out]  dist  the distance to the triangle
/// \param [out]  u     barycentric coordinate of the intersection
/// \param [out]  v     barycentric coordinate of the intersection
/// \returns      a code indicating that the closest point:
///               - 0 does not exist (should not occur)
///               - 1 is \a a
///               - 2 is \a b
///               - 3 is on the edge from \a a to \a b
///               - 4 is \a c
///               - 5 is on the edge from \a a to \a c
///               - 6 is on the edge from \a b to \a c
///               - 7 is on the face of the triangle
/// Barycentric coordinates are u and v such that (1-u-v)*a + u*b + v*c =
/// p+dist*d
KWIVER_ALGO_CORE_EXPORT
unsigned char
mesh_triangle_closest_point(
  const vital::point_3d& p,
  const vital::point_3d& a,
  const vital::point_3d& b,
  const vital::point_3d& c,
  const vital::vector_3d& n,
  double& dist,
  double& u, double& v );

/// Find the closest point on the triangle to a reference point
///
/// Find the closest point on the triangle a,b,c to point p. Returns the
/// distance to the mesh from the point and the barycentric coordinates on
/// the triangle of the intersection.
///
/// \param [in]   p     reference point to get closest distance to
/// \param [in]   a     corner point of triangle
/// \param [in]   b     corner point of triangle
/// \param [in]   c     corner point of triangle
/// \param [out]  dist  the distance to the triangle
/// \param [out]  u     barycentric coordinate of the intersection
/// \param [out]  v     barycentric coordinate of the intersection
/// \returns      a code indicating that the closest point:
///               - 0 does not exist (should not occur)
///               - 1 is \a a
///               - 2 is \a b
///               - 3 is on the edge from \a a to \a b
///               - 4 is \a c
///               - 5 is on the edge from \a a to \a c
///               - 6 is on the edge from \a b to \a c
///               - 7 is on the face of the triangle
/// Barycentric coordinates are u and v such that (1-u-v)*a + u*b + v*c =
/// p+dist*d
KWIVER_ALGO_CORE_EXPORT
unsigned char
mesh_triangle_closest_point(
  const vital::point_3d& p,
  const vital::point_3d& a,
  const vital::point_3d& b,
  const vital::point_3d& c,
  double& dist,
  double& u, double& v );

/// Find the closest point on the triangle to a reference point
///
/// Find the closest point on the triangle a,b,c to point p. Returns the
/// distance to the mesh from the point and the closest point.
///
/// \param [in]   p     reference point to get closest distance to
/// \param [in]   a     corner point of triangle
/// \param [in]   b     corner point of triangle
/// \param [in]   c     corner point of triangle
/// \param [out]  dist  the closest distance to the triangle
/// \returns      the point on the triangle closest to the reference point
KWIVER_ALGO_CORE_EXPORT
vital::point_3d
mesh_triangle_closest_point(
  const vital::point_3d& p,
  const vital::point_3d& a,
  const vital::point_3d& b,
  const vital::point_3d& c,
  double& dist );

/// Find the closest point on a triangulated mesh to a reference point
///
/// Find the closest point on the triangulated mesh to point p. Returns the
/// distance to the mesh from the point, the index of the mesh triangle
/// that the closest point is on, the closest point, and the barycentric
/// coordinates of the intersection on the triangle.
///
/// \param [in]   p     reference point to get closest distance to
/// \param [in]   mesh  the mesh
/// \param [out]  cp    the closest point on the mesh
/// \param [out]  u     barycentric coordinate of the intersection
/// \param [out]  v     barycentric coordinate of the intersection
/// \returns      the face index of the closest triangle (one of them
///               if on an edge or vertex). If the operation failed or is
///               not possible -1 is returned.
/// Barycentric coordinates are u and v such that (1-u-v)*a + u*b + v*c =
/// p+dist*d
KWIVER_ALGO_CORE_EXPORT
int
mesh_closest_point(
  const vital::point_3d& p,
  const vital::mesh& mesh,
  vital::point_3d& cp,
  double& u, double& v );

/// Intersect a ray from point to a triangulated mesh
///
/// Intersect the ray from point p with direction d and a triangulated mesh.
/// Returns the distance to the mesh from the point, the index of the mesh
/// triangle that the closest point is on, and the barycentric coordinates
/// of the intersection on the triangle.
///
/// \param [in]   p     point that is the start of the ray
/// \param [in]   d     direction of the ray
/// \param [in]   mesh  the mesh
/// \param [out]  dist  the distance to the mesh
/// \param [out]  u     barycentric coordinate of the intersection
/// \param [out]  v     barycentric coordinate of the intersection
/// \returns      the face index of the intersected triangle (one of them
///               if on an edge or vertex). If the operation failed or is
///               not possible -1 is returned.
/// Barycentric coordinates are u and v such that (1-u-v)*a + u*b + v*c =
/// p+dist*d
KWIVER_ALGO_CORE_EXPORT
int
mesh_intersect(
  const vital::point_3d& p,
  const vital::vector_3d& d,
  const vital::mesh& mesh,
  double& dist,
  double& u, double& v );

} // namespace core

} // namespace arrows

} // namespace kwiver

#endif // KWIVER_ARROWS_CORE_MESH_INTERSECT_H
