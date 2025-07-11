// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief test reading video from a list of images.

#include <arrows/core/algo/interpolate_track_spline.h>
#include <vital/plugin_management/plugin_manager.h>

#include <gtest/gtest.h>

#include <vital/algo/algorithm.txx>

namespace kv = kwiver::vital;
namespace kac = kwiver::arrows::core;
namespace algo = kwiver::vital::algo;

// ----------------------------------------------------------------------------
int
main( int argc, char** argv )
{
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}

// ----------------------------------------------------------------------------
TEST ( interpolate_track_spline, create )
{
  using namespace kwiver::vital;

  plugin_manager::instance().load_all_plugins();

  EXPECT_NE( nullptr, create_algorithm< algo::interpolate_track >( "spline" ) );
}

namespace {

constexpr static auto FRAME_RATE = kv::time_usec_t{ 3000 };

// ----------------------------------------------------------------------------
void
add_track_state(
  kv::track_sptr track, kv::frame_id_t frame,
  kv::bounding_box_d bbox, double confidence )
{
  auto const time = frame * FRAME_RATE;
  auto d = std::make_shared< kv::detected_object >( bbox, confidence );
  track->append(
    std::make_shared< kv::object_track_state >( frame, time, d ) );
}

// ----------------------------------------------------------------------------
void
check_track_state(
  kv::track_sptr track, kv::frame_id_t frame,
  kv::bounding_box_d bbox, double confidence )
{
  SCOPED_TRACE( "At frame " + std::to_string( frame ) );

  auto const i = track->find( frame );
  ASSERT_NE( track->end(), i );

  auto state = std::dynamic_pointer_cast< kv::object_track_state >( *i );
  ASSERT_NE( nullptr, state );

  EXPECT_EQ( track, state->track() );
  EXPECT_EQ( frame, state->frame() );
  EXPECT_EQ( frame * FRAME_RATE, state->time() );

  ASSERT_NE( nullptr, state->detection() );

  auto const& actual_bbox = state->detection()->bounding_box();
  EXPECT_EQ( bbox.min_x(), actual_bbox.min_x() );
  EXPECT_EQ( bbox.max_x(), actual_bbox.max_x() );
  EXPECT_EQ( bbox.min_y(), actual_bbox.min_y() );
  EXPECT_EQ( bbox.max_y(), actual_bbox.max_y() );

  EXPECT_DOUBLE_EQ( confidence, state->detection()->confidence() );
}

} // end anonymous namespace

// ----------------------------------------------------------------------------
TEST ( interpolate_track_spline, linear )
{
  kac::interpolate_track_spline its;

  // Create input track
  class test_track_data : public kv::track_data {};

  auto data = std::make_shared< test_track_data >();
  auto key_track = kv::track::create( data );

  add_track_state( key_track, 10, { 150, 150, 200, 200 }, 1.0 );
  add_track_state( key_track, 20, { 250, 250, 300, 300 }, 1.0 );
  add_track_state( key_track, 30, { 150, 350, 200, 400 }, 0.5 );

  // Run interpolation
  auto new_track = its.interpolate( key_track );

  // Validate expected results
  EXPECT_EQ( data, new_track->data() );
  EXPECT_EQ( 21, new_track->size() );

  check_track_state( new_track, 11, { 160, 160, 210, 210 }, 0.82 );
  check_track_state( new_track, 15, { 200, 200, 250, 250 }, 0.5 );
  check_track_state( new_track, 18, { 230, 230, 280, 280 }, 0.68 );
  check_track_state( new_track, 25, { 200, 300, 250, 350 }, 0.375 );
}
