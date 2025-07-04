// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Implementation of wrapper functions in similarity transform
///        estimation algorithm definition.

#include <vital/algo/estimate_similarity_transform.h>

namespace kwiver {

namespace vital {

namespace algo {

estimate_similarity_transform
::estimate_similarity_transform()
{
  attach_logger( "algo.estimate_similarity_transform" );
}

// ----------------------------------------------------------------------------
/// Estimate the similarity transform between two corresponding sets of cameras
similarity_d
estimate_similarity_transform
::estimate_transform(
  std::vector< camera_perspective_sptr > const& from,
  std::vector< camera_perspective_sptr > const& to ) const
{
  std::vector< vector_3d > from_pts, to_pts;
  for( camera_perspective_sptr c : from )
  {
    from_pts.push_back( c->center() );
  }

  for( camera_perspective_sptr c : to )
  {
    to_pts.push_back( c->center() );
  }
  return this->estimate_transform( from_pts, to_pts );
}

// ----------------------------------------------------------------------------
/// Estimate the similarity transform between two corresponding sets of
/// landmarks.
similarity_d
estimate_similarity_transform
::estimate_transform(
  std::vector< landmark_sptr > const& from,
  std::vector< landmark_sptr > const& to ) const
{
  std::vector< vector_3d > from_pts, to_pts;
  for( landmark_sptr l : from )
  {
    from_pts.push_back( l->loc() );
  }

  for( landmark_sptr l : to )
  {
    to_pts.push_back( l->loc() );
  }
  return this->estimate_transform( from_pts, to_pts );
}

namespace {

// ----------------------------------------------------------------------------
/// Helper function for assigning camera/landmark map contents to point vectors
///
/// \tparam M      Map type whose value_type::second_type is a std::shared_ptr
/// \tparam afunc  Pointer to the accessor function in the object that is
///                contained in the std::shared_ptr.
///
/// \param from_map      map of type M of objects at \c from position
/// \param to_map        map of type M of objects at \c to position
/// \param from_pts      vector in which to store \c from points that have
///                      a corresponding \c to point.
/// \param to_pts        vector in which to store \c to points that have
///                      a corresponding \c from point.
template < typename M,
  vector_3d( M::value_type::second_type::element_type::* afunc )( ) const >
void
map_to_pts(
  M const& from_map, M const& to_map,
  std::vector< vector_3d >& from_pts,
  std::vector< vector_3d >& to_pts )
{
  typename M::const_iterator from_it = from_map.begin(),
    to_it   = to_map.begin();
  // STL map structure's stored data is ordered (binary search tree impl
  // O(from.size + to.size)
  while( from_it != from_map.end() && to_it != to_map.end() )
  {
    // increment the lesser of the two when the frame IDs don't match
    if( from_it->first > to_it->first )
    {
      ++to_it;
    }
    else if( from_it->first < to_it->first )
    {
      ++from_it;
    }
    else // equal
    {
      from_pts.push_back( ( ( *from_it->second ).*afunc )() );
      to_pts.push_back( ( ( *to_it->second ).*afunc )() );
      ++from_it;
      ++to_it;
    }
  }
}

} // namespace <anonymous>

// ----------------------------------------------------------------------------
/// Estimate the similarity transform between two corresponding camera maps
similarity_d
estimate_similarity_transform
::estimate_transform(
  camera_map_sptr const from,
  camera_map_sptr const to ) const
{
  // determine point pairings based on shared frame IDs
  std::vector< vector_3d > from_pts, to_pts;
  camera_map::map_camera_t from_map = from->cameras(),
    to_map = to->cameras();

  auto from_it = from_map.begin();
  auto to_it = to_map.begin();

  // STL map structure's stored data is ordered (binary search tree impl
  // O(from.size + to.size)
  while( from_it != from_map.end() && to_it != to_map.end() )
  {
    // increment the lesser of the two when the frame IDs don't match
    if( from_it->first > to_it->first )
    {
      ++to_it;
    }
    else if( from_it->first < to_it->first )
    {
      ++from_it;
    }
    else // equal
    {
      auto from_cam_ptr =
        std::dynamic_pointer_cast< camera_perspective >( from_it->second );
      auto to_cam_ptr =
        std::dynamic_pointer_cast< camera_perspective >( to_it->second );
      from_pts.push_back( from_cam_ptr->center() );
      to_pts.push_back( to_cam_ptr->center() );
      ++from_it;
      ++to_it;
    }
  }

  return this->estimate_transform( from_pts, to_pts );
}

// ----------------------------------------------------------------------------
/// Estimate the similarity transform between two corresponding landmark maps
similarity_d
estimate_similarity_transform
::estimate_transform(
  landmark_map_sptr const from,
  landmark_map_sptr const to ) const
{
  // determine point pairings based on shared frame IDs
  std::vector< vector_3d > from_pts, to_pts;
  landmark_map::map_landmark_t from_map = from->landmarks(),
    to_map = to->landmarks();
  map_to_pts< landmark_map::map_landmark_t, &landmark::loc >(
    from_map, to_map, from_pts, to_pts );
  return this->estimate_transform( from_pts, to_pts );
}

#undef VITAL_EST_MAP_TO_PTS

} // namespace algo

} // namespace vital

} // namespace kwiver
