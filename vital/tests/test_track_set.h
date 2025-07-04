// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
///
/// \brief Common tests for all track set implementations
///
/// These test functions are in a header rather than a source file so that
/// anyone who writes a new track_set_implementation can run the same tests.

#ifndef KWIVER_VITAL_TEST_TEST_TRACK_SET_H_
#define KWIVER_VITAL_TEST_TEST_TRACK_SET_H_

#include <vital/types/feature_track_set.h>
#include <vital/types/track_set.h>
#include <vital/vital_types.h>

#include <gtest/gtest.h>

#include <algorithm>

namespace testing {

// These should probably be PrintTo, but that doesn't work for _PRED*
// assertions. See also https://github.com/google/googletest/issues/1261.

// ----------------------------------------------------------------------------
template <>
Message&
Message::operator<<( std::set< int64_t > const& s )
{
  ( *ss_ ) << "{ ";
  for( auto const i : s )
  {
    ( *ss_ ) << i << ' ';
  }
  ( *ss_ ) << "}";
  return *this;
}

// ----------------------------------------------------------------------------
template <>
Message&
Message::operator<<( std::vector< kwiver::vital::track_sptr > const& v )
{
  ( *ss_ ) << "[\n";
  for( auto const t : v )
  {
    ( *ss_ ) << ::testing::PrintToString( t ) << std::endl;
  }
  ( *ss_ ) << "]";
  return *this;
}

} // namespace testing

namespace kwiver {

namespace vital {

namespace testing {

// ----------------------------------------------------------------------------
bool
compare_ids( std::set< int64_t > const& a, std::set< int64_t > const& b )
{
  return std::equal( a.begin(), a.end(), b.begin() );
}

#define EXPECT_IDS_EQ( a, b ) \
EXPECT_PRED2( ::kwiver::vital::testing::compare_ids, a, b )

// ----------------------------------------------------------------------------
bool
compare_tracks( std::vector< track_sptr > a, std::vector< track_sptr > b )
{
  std::sort( a.begin(), a.end() );
  std::sort( b.begin(), b.end() );
  return std::equal( a.begin(), a.end(), b.begin() );
}

#define EXPECT_TRACKS_EQ( a, b ) \
EXPECT_PRED2( ::kwiver::vital::testing::compare_tracks, a, b )

// ----------------------------------------------------------------------------
// Make a very small example track set
track_set_sptr
make_simple_track_set( frame_id_t starting_frame_id )
{
  unsigned track_id = 0;

  std::vector< track_sptr > test_tracks;

  auto test_state1 = std::make_shared< track_state >( starting_frame_id );
  auto test_state2 = std::make_shared< track_state >( starting_frame_id + 3 );
  auto test_state3 = std::make_shared< track_state >( starting_frame_id + 8 );

  test_tracks.push_back( track::create() );
  test_tracks.back()->append( test_state1 );
  test_tracks.back()->set_id( track_id++ );

  test_tracks.push_back( track::create() );
  test_tracks.back()->append( test_state1->clone() );
  test_tracks.back()->set_id( track_id++ );

  // skip some track ids
  track_id = 5;

  test_tracks.push_back( track::create() );
  test_tracks.back()->append( test_state2 );
  test_tracks.back()->set_id( track_id++ );

  test_tracks.push_back( track::create() );
  test_tracks.back()->append( test_state3 );
  test_tracks.back()->set_id( track_id++ );

  test_tracks[ 0 ]->append( test_state2->clone() );
  test_tracks[ 0 ]->append( test_state3->clone() );
  test_tracks[ 1 ]->append( test_state2->clone() );
  test_tracks[ 2 ]->append( test_state3->clone() );

  return std::make_shared< track_set >( test_tracks );
}

// ----------------------------------------------------------------------------
// Run the uint test for track merging

// This test assumes the tracks in the two sets correspond to those
// generated with the above make_simple_track_set() function with
// starting_frame_id arguments 1 and 2.

void
test_track_set_merge( track_set_sptr test_set_1, track_set_sptr test_set_2 )
{
  EXPECT_FALSE( test_set_1->empty() );
  ASSERT_EQ( 4, test_set_1->size() );

  EXPECT_FALSE( test_set_2->empty() );
  ASSERT_EQ( 4, test_set_2->size() );

  track_set_sptr test_set_1_copy = test_set_1->clone();
  EXPECT_FALSE( test_set_1_copy->empty() );
  ASSERT_EQ( 4, test_set_1_copy->size() );

  test_set_1->merge_in_other_track_set( test_set_2 );

  EXPECT_FALSE( test_set_1->empty() );
  ASSERT_EQ( 4, test_set_1->size() );

  auto tracks = test_set_1->tracks();
  // tracks are not guaranteed to be in the original order, so sort by id
  auto cmp = [](track_sptr t1, track_sptr t2){
               return t1->size() > t2->size();
             };
  std::sort( tracks.begin(), tracks.end(), cmp );

  EXPECT_EQ( 6, tracks[ 0 ]->size() );
  EXPECT_EQ( 4, tracks[ 1 ]->size() );
  EXPECT_EQ( 4, tracks[ 2 ]->size() );
  EXPECT_EQ( 2, tracks[ 3 ]->size() );
  EXPECT_EQ( 1, test_set_1->first_frame() );
  EXPECT_EQ( 10, test_set_1->last_frame() );

  // Test merge with appending
  test_set_1_copy->merge_in_other_track_set(
    test_set_2, clone_type::DEEP,
    true );

  EXPECT_FALSE( test_set_1_copy->empty() );
  ASSERT_EQ( 8, test_set_1_copy->size() );

  tracks = test_set_1_copy->tracks();
  std::sort( tracks.begin(), tracks.end(), cmp );

  EXPECT_EQ( 3, tracks[ 0 ]->size() );
  EXPECT_EQ( 3, tracks[ 1 ]->size() );
  EXPECT_EQ( 2, tracks[ 2 ]->size() );
  EXPECT_EQ( 2, tracks[ 3 ]->size() );
  EXPECT_EQ( 2, tracks[ 4 ]->size() );
  EXPECT_EQ( 2, tracks[ 5 ]->size() );
  EXPECT_EQ( 1, tracks[ 6 ]->size() );
  EXPECT_EQ( 1, tracks[ 7 ]->size() );
  EXPECT_EQ( 1, test_set_1_copy->first_frame() );
  EXPECT_EQ( 10, test_set_1_copy->last_frame() );
}

// ----------------------------------------------------------------------------
// Run the unit tests for track_set accessor functions
//
// This test assumes the tracks in the set correspond to those
// generated in the above make_simple_track_set() function.
void
test_track_set_accessors( track_set_sptr test_set )
{
  EXPECT_FALSE( test_set->empty() );
  ASSERT_EQ( 4, test_set->size() );

  auto tracks = test_set->tracks();
  EXPECT_TRUE( test_set->contains( tracks[ 0 ] ) );
  EXPECT_FALSE( test_set->contains( tracks[ 1 ]->clone() ) );

  EXPECT_EQ( 3, test_set->active_tracks( -1 ).size() );
  EXPECT_EQ( 3, test_set->active_tracks( 4 ).size() );
  EXPECT_EQ( 2, test_set->active_tracks( 1 ).size() );
  EXPECT_EQ( 1, test_set->inactive_tracks( 4 ).size() );
  EXPECT_EQ( 2, test_set->inactive_tracks( 1 ).size() );

  EXPECT_EQ( nullptr, test_set->get_track( 2 ) );
  EXPECT_EQ( 5, test_set->get_track( 5 )->id() );

  using frame_id_set = std::set< frame_id_t >;
  EXPECT_IDS_EQ(
    ( frame_id_set{ 1, 4, 9 } ),
    test_set->all_frame_ids() );

  using track_id_set = std::set< track_id_t >;
  EXPECT_IDS_EQ(
    ( track_id_set{ 0, 1, 5, 6 } ),
    test_set->all_track_ids() );

  EXPECT_EQ( 1, test_set->first_frame() );
  EXPECT_EQ( 9, test_set->last_frame() );
  EXPECT_EQ( 3, test_set->terminated_tracks( -1 ).size() );
  EXPECT_EQ( 1, test_set->new_tracks( 4 ).size() );
  EXPECT_EQ( 0, test_set->new_tracks( -2 ).size() );

  EXPECT_EQ( 0.5, test_set->percentage_tracked( -1, -6 ) );
  EXPECT_EQ( 0.0, test_set->percentage_tracked( 1, -10 ) );

  EXPECT_EQ( 2, test_set->num_active_tracks( 1 ) );

  track_id_set set = test_set->active_track_ids( -1 );
  EXPECT_EQ( 3, set.size() );
  EXPECT_TRUE( set.find( 0 ) != set.end() );
  EXPECT_TRUE( set.find( 5 ) != set.end() );
  EXPECT_TRUE( set.find( 6 ) != set.end() );

  EXPECT_EQ( 4, test_set->size() );
  EXPECT_FALSE( test_set->empty() );
}

// ----------------------------------------------------------------------------
// Run the unit tests for track_set modifier functions
//
// This test assumes the tracks in the set correspond to those
// generated in the above make_simple_track_set() function.
void
test_track_set_modifiers( track_set_sptr test_set )
{
  auto tracks = test_set->tracks();
  // tracks are not guaranteed to be in the original order, so sort by id
  auto cmp = [](track_sptr t1, track_sptr t2){ return t1->id() < t2->id(); };
  std::sort( tracks.begin(), tracks.end(), cmp );

  auto new_track = track::create();
  new_track->set_id( 10 );
  new_track->append( std::make_shared< track_state >( 10 ) );
  new_track->append( std::make_shared< track_state >( 11 ) );

  // Attempt to merge tracks with temporal overlap
  EXPECT_FALSE( test_set->merge_tracks( tracks[ 0 ], tracks[ 1 ] ) );

  // Test removal
  EXPECT_FALSE( test_set->remove( new_track ) ); // not in set
  EXPECT_TRUE( test_set->remove( tracks[ 1 ] ) );
  EXPECT_EQ( 3, test_set->size() );
  EXPECT_FALSE( test_set->contains( tracks[ 1 ] ) );

  EXPECT_FALSE( test_set->remove_frame_data( -1 ) );

  // Attempt to merge a track not in the set
  EXPECT_FALSE( test_set->merge_tracks( new_track, tracks[ 0 ] ) );

  // Test insertion
  test_set->insert( new_track );
  EXPECT_TRUE( test_set->contains( new_track ) );
  EXPECT_EQ( 4, test_set->size() );

  // Attempt to merge tracks in the wrong order
  EXPECT_FALSE( test_set->merge_tracks( tracks[ 0 ], new_track ) );

  // Test merging
  EXPECT_TRUE( test_set->merge_tracks( new_track, tracks[ 0 ] ) );
  EXPECT_EQ( 3, test_set->size() );
  EXPECT_FALSE( test_set->contains( new_track ) );
  EXPECT_TRUE( new_track->empty() );
  EXPECT_NE( nullptr, new_track->data() );
  EXPECT_EQ( 5, tracks[ 0 ]->size() );

  [ & ]{
    auto const tdr =
      std::dynamic_pointer_cast< track_data_redirect >( new_track->data() );
    ASSERT_NE( nullptr, tdr );
    EXPECT_EQ( tracks[ 0 ], tdr->redirect_track );
    std::cout << "redirect ID: " << tdr->redirect_track->id() << std::endl;
  }();

  // Test merging through a redirect
  auto new_track2 = track::create();
  new_track2->set_id( 11 );
  new_track2->append( std::make_shared< track_state >( 12 ) );
  new_track2->append( std::make_shared< track_state >( 13 ) );
  test_set->insert( new_track2 );

  EXPECT_TRUE( test_set->merge_tracks( new_track2, new_track ) );

  // Apply a new frame data map to track_set
  track_set_frame_data_sptr data = std::make_shared< feature_track_set_frame_data >();
  track_set_frame_data_map_t data_map = { { 1, data } };
  test_set->set_frame_data( data_map );

  // Test shallow and deep clones
  auto test_set_deep = test_set->clone( clone_type::DEEP );
  auto test_set_shallow = test_set_deep->clone( clone_type::SHALLOW );
  EXPECT_EQ( test_set->size(), test_set_deep->size() );
  EXPECT_EQ( test_set->size(), test_set_shallow->size() );

  track_set_frame_data_map_t frame_data_map = test_set->all_frame_data();
  EXPECT_EQ( frame_data_map.size(), test_set_deep->all_frame_data().size() );

  test_set_deep->remove_frame_data( 1 );
  EXPECT_NE( frame_data_map.size(), test_set_deep->all_frame_data().size() );
  EXPECT_EQ( frame_data_map.size(), test_set_shallow->all_frame_data().size() );

  EXPECT_TRUE( test_set_deep->set_frame_data( frame_data_map ) );
  EXPECT_EQ( frame_data_map.size(), test_set_deep->all_frame_data().size() );
  EXPECT_EQ( frame_data_map.size(), test_set_shallow->all_frame_data().size() );
}

} // end namespace testing

} // end namespace vital

} // end namespace kwiver

#endif
