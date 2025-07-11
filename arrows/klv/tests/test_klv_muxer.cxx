// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// Test KLV muxer.

#include <arrows/klv/klv_all.h>
#include <arrows/klv/klv_demuxer.h>
#include <arrows/klv/klv_muxer.h>

#include <tests/test_gtest.h>

// ----------------------------------------------------------------------------
int
main( int argc, char** argv )
{
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}

using namespace kwiver::arrows::klv;
namespace kv = kwiver::vital;
using kld = klv_lengthy< double >;

// ----------------------------------------------------------------------------
class klv_muxer_test : public ::testing::Test
{
protected:
  klv_muxer_test()
    : src_timeline{},
      dst_timeline{},
      standard{ KLV_PACKET_UNKNOWN },
      index{},
      timestamps{
        kv::timestamp{ 100, 1 },
        kv::timestamp{ 110, 2 },
        kv::timestamp{ 120, 3 },
        kv::timestamp{ 130, 4 },
        kv::timestamp{ 140, 5 } }
  {}

  void
  SetUp()
  {
    standard = KLV_PACKET_MISB_0601_LOCAL_SET;
    index = klv_value{};
    add_src( KLV_0601_PLATFORM_HEADING_ANGLE, { 90, 115 },  kld{ 30.0 } );
    add_src( KLV_0601_PLATFORM_HEADING_ANGLE, { 125, 145 }, kld{ 40.0 } );
    add_src( KLV_0601_PLATFORM_PITCH_ANGLE,   { 101, 140 }, kld{ -11.0 } );
    add_src( KLV_0601_PLATFORM_ROLL_ANGLE,    { 121, 130 }, kld{ 9.0 } );
    add_src( KLV_0601_PLATFORM_ROLL_ANGLE,    { 131, 140 }, kld{ 8.0 } );

    add_dst( KLV_0601_PLATFORM_HEADING_ANGLE, { 100, 120 },      kld{ 30.0 } );
    add_dst( KLV_0601_PLATFORM_HEADING_ANGLE, { 130, 30000140 }, kld{ 40.0 } );
    add_dst( KLV_0601_PLATFORM_PITCH_ANGLE,   { 110, 140 },      kld{ -11.0 } );

    klv_1108_metric_implementer const implementer{ "Kitware", "KWIVER" };
    standard = KLV_PACKET_MISB_1108_LOCAL_SET;
    index = klv_local_set{
      { KLV_1108_ASSESSMENT_POINT, KLV_1108_ASSESSMENT_POINT_ARCHIVE },
      { KLV_1108_METRIC_LOCAL_SET,
        klv_local_set{
          { KLV_1108_METRIC_SET_NAME,        std::string{ "GSD" } },
          { KLV_1108_METRIC_SET_VERSION,     std::string{ "" } },
          { KLV_1108_METRIC_SET_IMPLEMENTER, implementer } } } };

    add_src(
      KLV_1108_ASSESSMENT_POINT, { 110, 135 },
      KLV_1108_ASSESSMENT_POINT_ARCHIVE );
    add_src(
      KLV_1108_METRIC_LOCAL_SET, { 110, 135 },
      klv_local_set{
      { KLV_1108_METRIC_SET_NAME,        std::string{ "GSD" } },
      { KLV_1108_METRIC_SET_VERSION,     std::string{ "" } },
      { KLV_1108_METRIC_SET_IMPLEMENTER, implementer },
      { KLV_1108_METRIC_SET_TIME,        uint64_t{ 123456 } },
      { KLV_1108_METRIC_SET_VALUE,       kld{ 20.0 } } } );
    add_src(
      KLV_1108_COMPRESSION_TYPE,    { 110, 135 },
      KLV_1108_COMPRESSION_TYPE_H264 );
    add_src(
      KLV_1108_COMPRESSION_PROFILE, { 110, 135 },
      KLV_1108_COMPRESSION_PROFILE_MAIN );
    add_src( KLV_1108_COMPRESSION_LEVEL,   { 110, 135 }, std::string{ "5.1" } );
    add_src( KLV_1108_COMPRESSION_RATIO,   { 110, 135 }, kld{ 22.0 }  );
    add_src( KLV_1108_STREAM_BITRATE,      { 110, 135 }, uint64_t{ 26 } );
    add_src( KLV_1108_DOCUMENT_VERSION,    { 110, 135 }, uint64_t{ 3 } );

    add_dst(
      KLV_1108_ASSESSMENT_POINT, { 110, 135 },
      KLV_1108_ASSESSMENT_POINT_ARCHIVE );
    add_dst(
      KLV_1108_METRIC_LOCAL_SET, { 110, 135 },
      klv_local_set{
      { KLV_1108_METRIC_SET_NAME,        std::string{ "GSD" } },
      { KLV_1108_METRIC_SET_VERSION,     std::string{ "" } },
      { KLV_1108_METRIC_SET_IMPLEMENTER, implementer },
      { KLV_1108_METRIC_SET_TIME,        uint64_t{ 123456 } },
      { KLV_1108_METRIC_SET_VALUE,       kld{ 20.0 } } } );
    add_dst(
      KLV_1108_COMPRESSION_TYPE,    { 110, 135 },
      KLV_1108_COMPRESSION_TYPE_H264 );
    add_dst(
      KLV_1108_COMPRESSION_PROFILE, { 110, 135 },
      KLV_1108_COMPRESSION_PROFILE_MAIN );
    add_dst( KLV_1108_COMPRESSION_LEVEL,   { 110, 135 }, std::string{ "5.1" } );
    add_dst( KLV_1108_COMPRESSION_RATIO,   { 110, 135 }, kld{ 22.0 }  );
    add_dst( KLV_1108_STREAM_BITRATE,      { 110, 135 }, uint64_t{ 26 } );
    add_dst( KLV_1108_DOCUMENT_VERSION,    { 110, 135 }, uint64_t{ 3 } );

    standard = KLV_PACKET_MISB_1108_LOCAL_SET;
    index = klv_local_set{
      { KLV_1108_ASSESSMENT_POINT, KLV_1108_ASSESSMENT_POINT_ARCHIVE },
      { KLV_1108_METRIC_LOCAL_SET,
        klv_local_set{
          { KLV_1108_METRIC_SET_NAME,        std::string{ "VNIIRS" } },
          { KLV_1108_METRIC_SET_VERSION,     std::string{ "1.0" } },
          { KLV_1108_METRIC_SET_IMPLEMENTER, implementer } } } };

    add_src(
      KLV_1108_ASSESSMENT_POINT, { 110, 155 },
      KLV_1108_ASSESSMENT_POINT_ARCHIVE );
    add_src(
      KLV_1108_METRIC_LOCAL_SET, { 110, 155 },
      klv_local_set{
      { KLV_1108_METRIC_SET_NAME,        std::string{ "VNIIRS" } },
      { KLV_1108_METRIC_SET_VERSION,     std::string{ "1.0" } },
      { KLV_1108_METRIC_SET_IMPLEMENTER, implementer },
      { KLV_1108_METRIC_SET_TIME,        uint64_t{ 123456 } },
      { KLV_1108_METRIC_SET_VALUE,       kld{ 5.0 } }, } );
    add_src(
      KLV_1108_COMPRESSION_TYPE,    { 110, 155 },
      KLV_1108_COMPRESSION_TYPE_H264 );
    add_src(
      KLV_1108_COMPRESSION_PROFILE, { 110, 155 },
      KLV_1108_COMPRESSION_PROFILE_MAIN );
    add_src( KLV_1108_COMPRESSION_LEVEL,   { 110, 155 }, std::string{ "5.1" } );
    add_src( KLV_1108_COMPRESSION_RATIO,   { 110, 155 }, kld{ 22.0 } );
    add_src( KLV_1108_STREAM_BITRATE,      { 110, 155 }, uint64_t{ 26 } );
    add_src( KLV_1108_DOCUMENT_VERSION,    { 110, 155 }, uint64_t{ 3 } );

    add_dst(
      KLV_1108_ASSESSMENT_POINT, { 110, 140 },
      KLV_1108_ASSESSMENT_POINT_ARCHIVE );
    add_dst(
      KLV_1108_METRIC_LOCAL_SET, { 110, 140 },
      klv_local_set{
      { KLV_1108_METRIC_SET_NAME,        std::string{ "VNIIRS" } },
      { KLV_1108_METRIC_SET_VERSION,     std::string{ "1.0" } },
      { KLV_1108_METRIC_SET_IMPLEMENTER, implementer },
      { KLV_1108_METRIC_SET_TIME,        uint64_t{ 123456 } },
      { KLV_1108_METRIC_SET_VALUE,       kld{ 5.0 } }, } );
    add_dst(
      KLV_1108_COMPRESSION_TYPE,    { 110, 140 },
      KLV_1108_COMPRESSION_TYPE_H264 );
    add_dst(
      KLV_1108_COMPRESSION_PROFILE, { 110, 140 },
      KLV_1108_COMPRESSION_PROFILE_MAIN );
    add_dst( KLV_1108_COMPRESSION_LEVEL,   { 110, 140 }, std::string{ "5.1" } );
    add_dst( KLV_1108_COMPRESSION_RATIO,   { 110, 140 }, kld{ 22.0 } );
    add_dst( KLV_1108_STREAM_BITRATE,      { 110, 140 }, uint64_t{ 26 } );
    add_dst( KLV_1108_DOCUMENT_VERSION,    { 110, 140 }, uint64_t{ 3 } );
  }

  void
  add_src(
    klv_lds_key tag, klv_timeline::interval_t time_interval,
    klv_value const& value )
  {
    src_timeline.insert_or_find( standard, tag, index )
    ->second.set( time_interval, value );
  }

  void
  add_dst(
    klv_lds_key tag, klv_timeline::interval_t time_interval,
    klv_value const& value )
  {
    dst_timeline.insert_or_find( standard, tag, index )
    ->second.set( time_interval, value );
  }

  // Test timeline -> packets -> timeline -> packets.
  // Packets are requested after all frames have been sent.
  void
  test_round_trip_buffered( klv_muxer& muxer, klv_timeline& new_timeline )
  {
    // Turn timeline into packets
    std::vector< klv_timed_packet > packets1;
    for( auto const& timestamp : timestamps )
    {
      muxer.send_frame( timestamp.get_time_usec() );
    }
    for( auto const& timestamp : timestamps )
    {
      for( auto const& packet : muxer.receive_frame() )
      {
        packets1.emplace_back( klv_timed_packet{ packet, timestamp } );
      }
    }

    // Packets back into timeline
    klv_demuxer demuxer( new_timeline );
    for( auto const& packet : packets1 )
    {
      demuxer.send_frame( { packet.packet } );
    }

    // Compare timelines
    EXPECT_EQ( dst_timeline, new_timeline )
      << "\n" << dst_timeline << "\n\n" << new_timeline << "\n";

    // And timeline back into packets again
    klv_muxer muxer2( new_timeline );
    muxer2.set_update_intervals( muxer.update_intervals() );

    std::vector< klv_timed_packet > packets2;
    for( auto const& timestamp : timestamps )
    {
      muxer2.send_frame( timestamp.get_time_usec() );
    }
    for( auto const& timestamp : timestamps )
    {
      for( auto const& packet : muxer2.receive_frame() )
      {
        packets2.emplace_back( klv_timed_packet{ packet, timestamp } );
      }
    }

    // Check that both sets of packets are the same
    EXPECT_EQ( packets1, packets2 );

    packets = packets1;
  }

  // Test timeline -> packets -> timeline -> packets.
  // Packets are requested immediately after each frame.
  void
  test_round_trip_immediate( klv_muxer& muxer, klv_timeline& new_timeline )
  {
    // Turn timeline into packets
    std::vector< klv_timed_packet > packets1;
    for( auto const& timestamp : timestamps )
    {
      muxer.send_frame( timestamp.get_time_usec() );
      for( auto const& packet : muxer.receive_frame() )
      {
        packets1.emplace_back( klv_timed_packet{ packet, timestamp } );
      }
    }

    // Packets back into timeline
    klv_demuxer demuxer( new_timeline );
    for( auto const& packet : packets1 )
    {
      demuxer.send_frame( { packet.packet } );
    }

    // Compare timelines
    EXPECT_EQ( dst_timeline, new_timeline )
      << "\n" << dst_timeline << "\n\n" << new_timeline << "\n";

    // And timeline back into packets again
    klv_muxer muxer2( new_timeline );
    muxer2.set_update_intervals( muxer.update_intervals() );

    std::vector< klv_timed_packet > packets2;
    for( auto const& timestamp : timestamps )
    {
      muxer2.send_frame( timestamp.get_time_usec() );
      for( auto const& packet : muxer2.receive_frame() )
      {
        packets2.emplace_back( klv_timed_packet{ packet, timestamp } );
      }
    }

    // Check that both sets of packets are the same
    EXPECT_EQ( packets1, packets2 );

    packets = packets1;
  }

  size_t
  count_1108_packets() const
  {
    size_t result = 0;
    for( auto const& packet : packets )
    {
      if( packet.packet.key == klv_1108_key() )
      {
        ++result;
      }
    }
    return result;
  }

  size_t
  count_0601_tag( klv_0601_tag tag ) const
  {
    size_t result = 0;
    for( auto const& packet : packets )
    {
      if( packet.packet.key == klv_0601_key() )
      {
        result += packet.packet.value.get< klv_local_set >().count( tag );
      }
    }
    return result;
  }

  klv_timeline src_timeline;
  klv_timeline dst_timeline;
  klv_top_level_tag standard;
  klv_value index;
  std::vector< kv::timestamp > timestamps;
  std::vector< klv_timed_packet > packets;
};

// ----------------------------------------------------------------------------
// Test muxing with the default update settings.
// These test results should be identical to the min_intervals test.
TEST_F ( klv_muxer_test, standard )
{
  // Do nothing to the muxer
  klv_muxer muxer( src_timeline );

  {
    klv_timeline new_timeline;
    CALL_TEST( test_round_trip_buffered, muxer, new_timeline );

    // Ensure the packets look generally how we would expect
    EXPECT_EQ( 4, count_1108_packets() );
    EXPECT_EQ( 5, count_0601_tag( KLV_0601_PLATFORM_HEADING_ANGLE ) );
    EXPECT_EQ( 4, count_0601_tag( KLV_0601_PLATFORM_PITCH_ANGLE ) );
  }

  muxer.reset();
  {
    klv_timeline new_timeline;
    CALL_TEST( test_round_trip_immediate, muxer, new_timeline );

    // Ensure the packets look generally how we would expect
    EXPECT_EQ( 4, count_1108_packets() );
    EXPECT_EQ( 5, count_0601_tag( KLV_0601_PLATFORM_HEADING_ANGLE ) );
    EXPECT_EQ( 4, count_0601_tag( KLV_0601_PLATFORM_PITCH_ANGLE ) );
  }
}

// ----------------------------------------------------------------------------
// Test muxing with the minumum-interval update settings.
TEST_F ( klv_muxer_test, min_intervals )
{
  // Set the muxer to be as verbose as possible
  klv_muxer muxer( src_timeline );
  klv_update_intervals intervals;
  intervals.set( KLV_UPDATE_INTERVAL_MIN );
  muxer.set_update_intervals( intervals );

  {
    klv_timeline new_timeline;
    CALL_TEST( test_round_trip_buffered, muxer, new_timeline );

    // Ensure the packets look generally how we would expect
    EXPECT_EQ( 4, count_1108_packets() );
    EXPECT_EQ( 5, count_0601_tag( KLV_0601_PLATFORM_HEADING_ANGLE ) );
    EXPECT_EQ( 4, count_0601_tag( KLV_0601_PLATFORM_PITCH_ANGLE ) );
  }

  muxer.reset();
  {
    klv_timeline new_timeline;
    CALL_TEST( test_round_trip_immediate, muxer, new_timeline );

    // Ensure the packets look generally how we would expect
    EXPECT_EQ( 4, count_1108_packets() );
    EXPECT_EQ( 5, count_0601_tag( KLV_0601_PLATFORM_HEADING_ANGLE ) );
    EXPECT_EQ( 4, count_0601_tag( KLV_0601_PLATFORM_PITCH_ANGLE ) );
  }
}

// ----------------------------------------------------------------------------
// Test muxing with the maximum-interval update settings.
TEST_F ( klv_muxer_test, max_intervals )
{
  // Remove the one (acceptable) discrepancy from the reference timeline
  dst_timeline.find(
    KLV_PACKET_MISB_0601_LOCAL_SET,
    KLV_0601_PLATFORM_HEADING_ANGLE )->second.erase( { 30000130, 30000140 } );

  // Set the muxer to be as sparse as possible
  klv_muxer muxer( src_timeline );
  klv_update_intervals intervals;
  intervals.set( KLV_UPDATE_INTERVAL_MAX );
  muxer.set_update_intervals( intervals );

  {
    klv_timeline new_timeline;
    CALL_TEST( test_round_trip_buffered, muxer, new_timeline );

    // Ensure the packets look generally how we would expect
    EXPECT_EQ( 3, count_1108_packets() );
    EXPECT_EQ( 3, count_0601_tag( KLV_0601_PLATFORM_HEADING_ANGLE ) );
    EXPECT_EQ( 2, count_0601_tag( KLV_0601_PLATFORM_PITCH_ANGLE ) );
  }

  muxer.reset();
  {
    klv_timeline new_timeline;
    CALL_TEST( test_round_trip_immediate, muxer, new_timeline );

    // Ensure the packets look generally how we would expect
    EXPECT_EQ( 4, count_1108_packets() );
    EXPECT_EQ( 3, count_0601_tag( KLV_0601_PLATFORM_HEADING_ANGLE ) );
    EXPECT_EQ( 2, count_0601_tag( KLV_0601_PLATFORM_PITCH_ANGLE ) );
  }
}

// ----------------------------------------------------------------------------
TEST_F ( klv_muxer_test, mixed_intervals )
{
  // Remove the one (acceptable) discrepancy from the reference timeline
  dst_timeline.find(
    KLV_PACKET_MISB_0601_LOCAL_SET,
    KLV_0601_PLATFORM_HEADING_ANGLE )->second.erase( { 30000130, 30000140 } );

  // Set the muxer to different settings for different domains
  klv_muxer muxer( src_timeline );
  klv_update_intervals intervals;
  intervals.set(
    { KLV_PACKET_MISB_0601_LOCAL_SET, KLV_0601_PLATFORM_HEADING_ANGLE },
    KLV_UPDATE_INTERVAL_MAX );
  intervals.set(
    { KLV_PACKET_MISB_0601_LOCAL_SET, KLV_0601_PLATFORM_PITCH_ANGLE }, 10 );
  intervals.set( KLV_PACKET_MISB_1108_LOCAL_SET, 10 );
  muxer.set_update_intervals( intervals );

  {
    klv_timeline new_timeline;
    CALL_TEST( test_round_trip_buffered, muxer, new_timeline );

    // Ensure the packets look generally how we would expect
    EXPECT_EQ( 4, count_1108_packets() );
    EXPECT_EQ( 3, count_0601_tag( KLV_0601_PLATFORM_HEADING_ANGLE ) );
    EXPECT_EQ( 4, count_0601_tag( KLV_0601_PLATFORM_PITCH_ANGLE ) );
  }

  muxer.reset();
  {
    klv_timeline new_timeline;
    CALL_TEST( test_round_trip_immediate, muxer, new_timeline );

    // Ensure the packets look generally how we would expect
    EXPECT_EQ( 4, count_1108_packets() );
    EXPECT_EQ( 3, count_0601_tag( KLV_0601_PLATFORM_HEADING_ANGLE ) );
    EXPECT_EQ( 4, count_0601_tag( KLV_0601_PLATFORM_PITCH_ANGLE ) );
  }
}
