// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <test_tracks.h>

#include <arrows/core/track_set_impl.h>
#include <vital/tests/test_track_set.h>
#include <vital/types/feature_track_set.h>

using namespace kwiver::vital;

// ----------------------------------------------------------------------------
int
main( int argc, char** argv )
{
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}

namespace {

// ----------------------------------------------------------------------------
track_set_sptr
make_track_set_impl( std::vector< track_sptr > const& tracks )
{
  auto tsi = std::unique_ptr< track_set_implementation >{
    new kwiver::arrows::core::frame_index_track_set_impl{ tracks } };
  return std::make_shared< track_set >( std::move( tsi ) );
}

} // namespace

// ----------------------------------------------------------------------------
TEST ( frame_index_track_set_impl, accessor_functions )
{
  auto test_set = kwiver::vital::testing::make_simple_track_set( 1 );

  test_set = make_track_set_impl( test_set->tracks() );

  kwiver::vital::testing::test_track_set_accessors( test_set );
}

// ----------------------------------------------------------------------------
TEST ( frame_index_track_set_impl, modifier_functions )
{
  auto test_set = kwiver::vital::testing::make_simple_track_set( 1 );

  test_set = make_track_set_impl( test_set->tracks() );

  kwiver::vital::testing::test_track_set_modifiers( test_set );
}

// ----------------------------------------------------------------------------
TEST ( frame_index_track_set_impl, matches_simple )
{
  auto tracks = kwiver::testing::generate_tracks();

  auto ftracks = make_track_set_impl( tracks->tracks() );

  EXPECT_EQ( tracks->size(), ftracks->size() );
  EXPECT_EQ( tracks->empty(), ftracks->empty() );
  EXPECT_EQ( tracks->first_frame(), ftracks->first_frame() );
  EXPECT_EQ( tracks->last_frame(), ftracks->last_frame() );

  auto all_frames_s = tracks->all_frame_ids();
  auto all_frames_f = ftracks->all_frame_ids();
  EXPECT_TRUE(
    std::equal(
      all_frames_s.begin(), all_frames_s.end(),
      all_frames_f.begin() ) );

  EXPECT_IDS_EQ( tracks->all_track_ids(), ftracks->all_track_ids() );
  EXPECT_TRACKS_EQ(
    tracks->active_tracks( 5 ),
    ftracks->active_tracks( 5 ) );
  EXPECT_TRACKS_EQ(
    tracks->inactive_tracks( 15 ),
    ftracks->inactive_tracks( 15 ) );
  EXPECT_TRACKS_EQ(
    tracks->new_tracks( 40 ),
    ftracks->new_tracks( 40 ) );
  EXPECT_TRACKS_EQ(
    tracks->terminated_tracks( 60 ),
    ftracks->terminated_tracks( 60 ) );
  EXPECT_EQ(
    tracks->percentage_tracked( 10, 50 ),
    ftracks->percentage_tracked( 10, 50 ) );
}

// ----------------------------------------------------------------------------
TEST ( frame_index_track_set_impl, remove_frame_data )
{
  auto test_set = kwiver::vital::testing::make_simple_track_set( 1 );

  test_set = make_track_set_impl( test_set->tracks() );

  auto fd1 = std::make_shared< feature_track_set_frame_data >();
  fd1->is_keyframe = true;

  auto td1 = std::static_pointer_cast< track_set_frame_data >( fd1 );
  EXPECT_EQ( 0, test_set->all_frame_data().size() );
  test_set->set_frame_data( td1, 1 );
  EXPECT_EQ( 1, test_set->all_frame_data().size() );
  test_set->remove_frame_data( 1 );
  EXPECT_EQ( 0, test_set->all_frame_data().size() );
}

// ----------------------------------------------------------------------------
TEST ( frame_index_track_set_impl, merge_functions )
{
  using namespace kwiver::vital::testing;

  auto test_set_1 = kwiver::vital::testing::make_simple_track_set( 1 );
  test_set_1 = make_track_set_impl( test_set_1->tracks() );

  auto test_set_2 = kwiver::vital::testing::make_simple_track_set( 2 );
  test_set_2 = make_track_set_impl( test_set_2->tracks() );

  test_track_set_merge( test_set_1, test_set_2 );
}
