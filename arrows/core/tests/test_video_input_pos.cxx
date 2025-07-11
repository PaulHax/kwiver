// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief test reading video from a list of images.

#include <test_gtest.h>

#include <arrows/core/algo/video_input_pos.h>
#include <vital/algo/algorithm.txx>
#include <vital/io/metadata_io.h>
#include <vital/plugin_management/plugin_manager.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <string>

kwiver::vital::path_t g_data_dir;

namespace algo = kwiver::vital::algo;
namespace kac = kwiver::arrows::core;

static int num_expected_frames = 50;
static std::string list_file_name = "video_as_images/frame_list.txt";
static std::string pos_folder_location = "video_as_images/pos";

// ----------------------------------------------------------------------------
int
main( int argc, char* argv[] )
{
  ::testing::InitGoogleTest( &argc, argv );
  TEST_LOAD_PLUGINS();

  GET_ARG( 1, g_data_dir );

  return RUN_ALL_TESTS();
}

// ----------------------------------------------------------------------------
class video_input_pos : public ::testing::Test
{
  TEST_ARG( data_dir );
};

// ----------------------------------------------------------------------------
TEST_F ( video_input_pos, create )
{
  EXPECT_NE(
    nullptr,
    kwiver::vital::create_algorithm< kwiver::vital::algo::video_input >(
      "pos" ) );
}

// ----------------------------------------------------------------------------
TEST_F ( video_input_pos, read_list )
{
  // make config block
  auto config = kwiver::vital::config_block::empty_config();
  config->set_value(
    "metadata_directory",
    data_dir + "/" + pos_folder_location );

  kwiver::arrows::core::video_input_pos vip;

  vip.check_configuration( config );
  vip.set_configuration( config );

  kwiver::vital::path_t list_file = data_dir + "/" + list_file_name;
  vip.open( list_file );

  kwiver::vital::timestamp ts;

  int num_frames = 0;
  while( vip.next_frame( ts ) )
  {
    auto md = vip.frame_metadata();

    if( md.size() > 0 )
    {
      std::cout << "-----------------------------------\n" << std::endl;
      kwiver::vital::print_metadata( std::cout, *md[ 0 ] );
    }
    ++num_frames;
    EXPECT_EQ( num_frames, ts.get_frame() )
      << "Frame numbers should be sequential";

    EXPECT_EQ( ts.get_time_usec(), vip.frame_timestamp().get_time_usec() );
    EXPECT_EQ( ts.get_frame(), vip.frame_timestamp().get_frame() );
  }
  EXPECT_EQ( num_expected_frames, num_frames );
  EXPECT_EQ( num_expected_frames, vip.num_frames() );
}

// ----------------------------------------------------------------------------
TEST_F ( video_input_pos, is_good )
{
  // make config block
  auto config = kwiver::vital::config_block::empty_config();
  config->set_value(
    "metadata_directory",
    data_dir + "/" + pos_folder_location );

  kwiver::arrows::core::video_input_pos vip;

  EXPECT_TRUE( vip.check_configuration( config ) );
  vip.set_configuration( config );

  kwiver::vital::path_t list_file = data_dir + "/" + list_file_name;
  kwiver::vital::timestamp ts;

  EXPECT_FALSE( vip.good() )
    << "Video state before open";

  // open the video
  vip.open( list_file );
  EXPECT_FALSE( vip.good() )
    << "Video state after open but before first frame";

  // step one frame
  vip.next_frame( ts );
  EXPECT_TRUE( vip.good() )
    << "Video state on first frame";

  // close the video
  vip.close();
  EXPECT_FALSE( vip.good() )
    << "Video state after close";

  // Reopen the video
  vip.open( list_file );

  int num_frames = 0;
  while( vip.next_frame( ts ) )
  {
    ++num_frames;
    EXPECT_TRUE( vip.good() )
      << "Video state on frame " << ts.get_frame();
  }
  EXPECT_EQ( num_expected_frames, num_frames );
}

TEST_F ( video_input_pos, seek_frame )
{
  // make config block
  auto config = kwiver::vital::config_block::empty_config();
  config->set_value(
    "metadata_directory",
    data_dir + "/" + pos_folder_location );

  kwiver::arrows::core::video_input_pos vip;

  EXPECT_TRUE( vip.check_configuration( config ) );
  vip.set_configuration( config );

  kwiver::vital::path_t list_file = data_dir + "/" + list_file_name;
  kwiver::vital::timestamp ts;

  // Open the video
  vip.open( list_file );

  // Video should be seekable
  ASSERT_TRUE( vip.seekable() );

  // Test various valid seeks
  std::vector< kwiver::vital::timestamp::frame_t > valid_seeks = { 3, 23, 46,
                                                                   34, 50, 1 };
  for( auto requested_frame : valid_seeks )
  {
    EXPECT_TRUE( vip.seek_frame( ts, requested_frame ) );
    EXPECT_EQ( requested_frame, ts.get_frame() );
  }

  // Test various invalid seeks past end of video
  std::vector< kwiver::vital::timestamp::frame_t > in_valid_seeks = { -3, -1,
                                                                      51, 55 };
  for( auto requested_frame : in_valid_seeks )
  {
    EXPECT_FALSE( vip.seek_frame( ts, requested_frame ) );
    EXPECT_NE( requested_frame, ts.get_frame() );
  }

  vip.close();
}

TEST_F ( video_input_pos, metadata_map )
{
  // make config block
  auto config = kwiver::vital::config_block::empty_config();
  config->set_value(
    "metadata_directory",
    data_dir + "/" + pos_folder_location );

  kwiver::arrows::core::video_input_pos vip;

  EXPECT_TRUE( vip.check_configuration( config ) );
  vip.set_configuration( config );

  kwiver::vital::path_t list_file = data_dir + "/" + list_file_name;
  kwiver::vital::timestamp ts;

  // Open the video
  vip.open( list_file );

  // Get metadata map
  auto md_map = vip.metadata_map()->metadata();

  EXPECT_EQ( md_map.size(), num_expected_frames )
    << "There should be metadata for every frame";

  // Open the list file directly and construct metadata file names and compare
  std::ifstream list_file_stream( list_file );
  int frame_number = 1;
  std::string file_name;
  while( std::getline( list_file_stream, file_name ) )
  {
    file_name.replace( 0, 6, pos_folder_location );
    file_name.replace( file_name.length() - 3, 3, "pos" );

    auto md_test = kwiver::vital::read_pos_file( data_dir + "/" + file_name );
    auto md = md_map[ frame_number ][ 0 ];

    // Loop over metadata items and compare
    for( auto iter = md_test->begin(); iter != md_test->end(); ++iter )
    {
      EXPECT_TRUE( md->has( iter->first ) )
        << "Metadata should have item " << iter->second->name();
    }

    frame_number++;
  }
  list_file_stream.close();

  vip.close();
}
