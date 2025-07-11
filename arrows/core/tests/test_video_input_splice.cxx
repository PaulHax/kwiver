// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief test reading images and metadata with video_input_splice

#include <test_gtest.h>

#include <arrows/core/algo/video_input_splice.h>
#include <arrows/tests/test_video_input.h>
#include <vital/algo/algorithm.txx>
#include <vital/algo/image_io.h>

#include <vital/io/metadata_io.h>
#include <vital/plugin_management/plugin_manager.h>
#include <vital/vital_config.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <string>

kwiver::vital::path_t g_data_dir;

namespace algo = kwiver::vital::algo;
namespace kac = kwiver::arrows::core;

static std::string source_list_file_name = "video_as_images/source_list.txt";
static std::string frame_list_file_name = "video_as_images/frame_list.txt";

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
class video_input_splice : public ::testing::Test
{
  TEST_ARG( data_dir );
};

// ----------------------------------------------------------------------------
TEST_F ( video_input_splice, create )
{
  EXPECT_NE(
    nullptr,
    kwiver::vital::create_algorithm< algo::video_input >( "splice" ) );
}

// ----------------------------------------------------------------------------
static
bool
set_config(
  kwiver::vital::config_block_sptr config,
  VITAL_UNUSED std::string const& data_dir )
{
  for( int n = 1; n < 4; ++n )
  {
    std::string block_name = "video_source_" + std::to_string( n ) + ":";

    config->set_value( block_name + "type", "image_list" );
    if( kwiver::vital::has_algorithm_impl_name< algo::image_io >( "ocv" ) )
    {
      config->set_value( block_name + "image_list:image_reader:type", "ocv" );
    }
    else if( kwiver::vital::has_algorithm_impl_name< algo::image_io >( "vxl" ) )
    {
      config->set_value( block_name + "image_list:image_reader:type", "vxl" );
    }
    else
    {
      std::cout << "Skipping tests since there is no image reader." <<
        std::endl;
      return false;
    }
  }

  return true;
}

// ----------------------------------------------------------------------------
TEST_F ( video_input_splice, is_good )
{
  // Make config block
  auto config = kwiver::vital::config_block::empty_config();

  if( !set_config( config, data_dir ) )
  {
    return;
  }

  kwiver::arrows::core::video_input_splice vis;

  EXPECT_TRUE( vis.check_configuration( config ) );
  vis.set_configuration( config );

  kwiver::vital::path_t list_file = data_dir + "/" + source_list_file_name;
  vis.open( list_file );

  kwiver::vital::timestamp ts;

  EXPECT_FALSE( vis.good() );

  // open the video
  vis.open( list_file );
  EXPECT_FALSE( vis.good() );

  // step one frame
  vis.next_frame( ts );
  EXPECT_TRUE( vis.good() );

  // close the video
  vis.close();
  EXPECT_FALSE( vis.good() );

  // Reopen the video
  vis.open( list_file );

  int num_frames = 0;
  while( vis.next_frame( ts ) )
  {
    ++num_frames;
    EXPECT_TRUE( vis.good() )
      << "Video state on frame " << ts.get_frame();
  }
  EXPECT_EQ( num_expected_frames, num_frames );

  EXPECT_FALSE( vis.good() );
}

// ----------------------------------------------------------------------------
TEST_F ( video_input_splice, next_frame )
{
  // Make config block
  auto config = kwiver::vital::config_block::empty_config();

  if( !set_config( config, data_dir ) )
  {
    return;
  }

  kwiver::arrows::core::video_input_splice vis;

  EXPECT_TRUE( vis.check_configuration( config ) );
  vis.set_configuration( config );

  kwiver::vital::path_t list_file = data_dir + "/" + source_list_file_name;
  vis.open( list_file );

  kwiver::vital::timestamp ts;

  int num_frames = 0;
  while( vis.next_frame( ts ) )
  {
    auto img = vis.frame_image();
    auto md = vis.frame_metadata();

    if( md.size() > 0 )
    {
      std::cout << "-----------------------------------\n" << std::endl;
      kwiver::vital::print_metadata( std::cout, *md[ 0 ] );
    }

    ++num_frames;
    EXPECT_EQ( num_frames, ts.get_frame() )
      << "Frame numbers should be sequential";
    EXPECT_EQ( ts.get_frame(), decode_barcode( *img ) )
      << "Frame number should match barcode in frame image";
  }

  EXPECT_FALSE( vis.next_frame( ts ) );
  EXPECT_TRUE( vis.end_of_video() );
  EXPECT_EQ( num_expected_frames, num_frames );
  EXPECT_EQ( num_expected_frames, vis.num_frames() );
}

// ----------------------------------------------------------------------------
TEST_F ( video_input_splice, seek_frame )
{
  // Make config block
  auto config = kwiver::vital::config_block::empty_config();

  if( !set_config( config, data_dir ) )
  {
    return;
  }

  kwiver::arrows::core::video_input_splice vis;

  EXPECT_TRUE( vis.check_configuration( config ) );
  vis.set_configuration( config );

  kwiver::vital::path_t list_file = data_dir + "/" + source_list_file_name;

  // Open the video
  vis.open( list_file );

  test_seek_frame( vis );

  vis.close();
}

// ----------------------------------------------------------------------------
TEST_F ( video_input_splice, seek_then_next_frame )
{
  // Make config block
  auto config = kwiver::vital::config_block::empty_config();

  if( !set_config( config, data_dir ) )
  {
    return;
  }

  kwiver::arrows::core::video_input_splice vis;

  EXPECT_TRUE( vis.check_configuration( config ) );
  vis.set_configuration( config );

  kwiver::vital::path_t list_file = data_dir + "/" + source_list_file_name;

  // Open the video
  vis.open( list_file );

  test_seek_then_next( vis );

  vis.close();
}

// ----------------------------------------------------------------------------
TEST_F ( video_input_splice, next_then_seek_frame )
{
  // Make config block
  auto config = kwiver::vital::config_block::empty_config();

  if( !set_config( config, data_dir ) )
  {
    return;
  }

  kwiver::arrows::core::video_input_splice vis;

  EXPECT_TRUE( vis.check_configuration( config ) );
  vis.set_configuration( config );

  kwiver::vital::path_t list_file = data_dir + "/" + source_list_file_name;

  // Open the video
  vis.open( list_file );

  test_next_then_seek( vis );

  vis.close();
}

// ----------------------------------------------------------------------------
TEST_F ( video_input_splice, next_then_seek_then_next )
{
  // Make config block
  auto config = kwiver::vital::config_block::empty_config();

  if( !set_config( config, data_dir ) )
  {
    return;
  }

  kwiver::arrows::core::video_input_splice vis;

  EXPECT_TRUE( vis.check_configuration( config ) );
  vis.set_configuration( config );

  kwiver::vital::path_t list_file = data_dir + "/" + source_list_file_name;

  // Open the video
  vis.open( list_file );

  test_next_then_seek_then_next( vis );

  vis.close();
}

// ----------------------------------------------------------------------------
TEST_F ( video_input_splice, metadata_map )
{
  // Make config block
  auto config = kwiver::vital::config_block::empty_config();

  if( !set_config( config, data_dir ) )
  {
    return;
  }

  kwiver::arrows::core::video_input_splice vis;

  EXPECT_TRUE( vis.check_configuration( config ) );
  vis.set_configuration( config );

  kwiver::vital::path_t list_file = data_dir + "/" + source_list_file_name;

  // Open the video
  vis.open( list_file );

  // Get metadata map
  auto md_map = vis.metadata_map()->metadata();

  EXPECT_EQ( md_map.size(), num_expected_frames )
    << "There should be metadata for every frame";

  // Open the frame list file directly and compare name to metadata
  std::ifstream list_file_stream( data_dir + "/" + frame_list_file_name );
  int frame_number = 1;
  std::string file_name;
  while( std::getline( list_file_stream, file_name ) )
  {
    auto md_file_name = md_map[ frame_number ][ 0 ]->find(
      kwiver::vital::VITAL_META_IMAGE_URI ).as_string();
    EXPECT_TRUE( md_file_name.find( file_name ) != std::string::npos )
      << "File path in metadata should contain " << file_name;
    frame_number++;
  }
  list_file_stream.close();

  vis.close();
}

// ----------------------------------------------------------------------------
TEST_F ( video_input_splice, next_frame_nth_frame_output )
{
  // Make config block
  auto config = kwiver::vital::config_block::empty_config();

  config->set_value( "output_nth_frame", nth_frame_output );

  if( !set_config( config, data_dir ) )
  {
    return;
  }

  kwiver::arrows::core::video_input_splice vis;

  EXPECT_TRUE( vis.check_configuration( config ) );
  vis.set_configuration( config );

  kwiver::vital::path_t list_file = data_dir + "/" + source_list_file_name;
  vis.open( list_file );

  test_read_video_nth_frame( vis );

  vis.close();
}

// ----------------------------------------------------------------------------
TEST_F ( video_input_splice, seek_frame_nth_frame_output )
{
  // Make config block
  auto config = kwiver::vital::config_block::empty_config();

  config->set_value( "output_nth_frame", nth_frame_output );

  if( !set_config( config, data_dir ) )
  {
    return;
  }

  kwiver::arrows::core::video_input_splice vis;

  EXPECT_TRUE( vis.check_configuration( config ) );
  vis.set_configuration( config );

  kwiver::vital::path_t list_file = data_dir + "/" + source_list_file_name;
  vis.open( list_file );

  test_seek_nth_frame( vis );

  vis.close();
}

// ----------------------------------------------------------------------------
TEST_F ( video_input_splice, test_capabilities )
{
  // Make config block
  auto config = kwiver::vital::config_block::empty_config();

  if( !set_config( config, data_dir ) )
  {
    return;
  }

  kwiver::arrows::core::video_input_splice vis;

  EXPECT_TRUE( vis.check_configuration( config ) );
  vis.set_configuration( config );

  kwiver::vital::path_t list_file = data_dir + "/" + source_list_file_name;
  vis.open( list_file );

  auto cap = vis.get_implementation_capabilities();
  auto cap_list = cap.capability_list();

  for( auto one : cap_list )
  {
    std::cout << one << " -- "
              << ( cap.capability( one ) ? "true" : "false" )
              << std::endl;
  }
}
// ----------------------------------------------------------------------------
TEST_F ( video_input_splice, test_config_file )
{
  auto config = kwiver::vital::config_block::empty_config();

  if( !set_config( config, data_dir ) )
  {
    return;
  }

  kwiver::arrows::core::video_input_splice vis;

  EXPECT_TRUE( vis.check_configuration( config ) );
  vis.set_configuration( config );

  auto vis_config = vis.get_configuration();
  auto diff = config->difference_config( vis_config );
  // set and get should produce the same configuration
  EXPECT_TRUE( diff->available_values().empty() );
}
