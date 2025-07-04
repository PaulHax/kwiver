// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
///
/// This program reads a video and extracts all the KLV metadata.

#include "dump_klv.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <regex>

#include <vital/algo/image_io.h>
#include <vital/algo/metadata_map_io.h>
#include <vital/algo/video_input.h>

#include <vital/plugin_management/plugin_manager.h>

#include <vital/config/config_block.h>
#include <vital/config/config_block_formatter.h>
#include <vital/config/config_block_io.h>

#include <vital/io/metadata_io.h>

#include <vital/util/get_paths.h>
#include <vital/util/thread_pool.h>
#include <vital/util/wrap_text_block.h>

#include <vital/types/metadata.h>
#include <vital/types/metadata_map.h>
#include <vital/types/metadata_traits.h>

#include <vital/exceptions.h>

#include <kwiversys/SystemTools.hxx>

#include <vital/algo/algorithm.txx>

namespace kv = kwiver::vital;
namespace kva = kwiver::vital::algo;

namespace kwiver {

namespace arrows {

namespace core {

// ----------------------------------------------------------------------------
void
dump_klv
::add_command_options()
{
  m_cmd_options->custom_help(
    wrap_text(
      "[options]  video-file\n"
      "This program displays the KLV metadata packets that are embedded "
      "in a video file."
    )
  );
  m_cmd_options->positional_help( "\n  video-file  - name of video file." );

  m_cmd_options->add_options()
  ( "h,help", "Display applet usage" )
  ( "c,config", "Configuration file for tool",
    cxxopts::value< std::string > () )
  ( "o,output", "Dump configuration to file and exit",
    cxxopts::value< std::string > () )
  ( "l,log",
    "Log metadata to a file. This requires the JSON serialization plugin. "
    "The file is structured as an array of frames where each frame contains an array "
    "of metadata packets associated with that frame. Each packet is an "
    "array of metadata fields. Alternatively, the configuration file, "
    "dump_klv.conf, can be updated to use CSV instead.",
    cxxopts::value< std::string > () )
  ( "frame-range",
    "Frame range to process, indexed from 1. May be a single number or two "
    "numbers separated by a hyphen, either of which may be omitted to process "
    "from the start or to the end of the video, respectively, e.g. '5', "
    "'10-100', or '64-'.",
    cxxopts::value< std::string > (), "expr" )
  ( "f,frames", "Dump frames into the given image format.",
    cxxopts::value< std::string > (), "extension" )
  ( "frames-dir", "Directory in which to dump frames. "
                  "Defaults to current directory.",
    cxxopts::value< std::string > (), "path" )
  ( "d,detail", "Display a detailed description of the metadata" )
  ( "q,quiet", "Do not show metadata. Overrides -d/--detail." )
  ( "e,exporter", "Choose the format of the exported KLV data. "
                  "Current options are: csv, json, klv-json.",
    cxxopts::value< std::string > (), "format" )
  ( "multithread",
    "Use multithreading to accelerate encoding of frame images. "
    "Number of worker threads is not configurable at this time." )
  ( "compress", "Compress output file. Only available for klv-json." )

  // positional parameters
  ( "video-file", "Video input file", cxxopts::value< std::string > () )
  ;

  m_cmd_options->parse_positional( "video-file" );
}

// ----------------------------------------------------------------------------
int
dump_klv
::run()
{
  const std::string opt_app_name = applet_name();
  std::string video_file;

  auto& cmd_args = command_args();

  if( cmd_args[ "help" ].as< bool >() )
  {
    std::cout << m_cmd_options->help();
    return EXIT_SUCCESS;
  }

  if( cmd_args.count( "video-file" ) )
  {
    video_file = cmd_args[ "video-file" ].as< std::string >();
  }
  else
  {
    std::cout << "Missing video file name.\n"
              << m_cmd_options->help();

    return EXIT_FAILURE;
  }

  kva::video_input_sptr video_reader;
  kva::metadata_map_io_sptr metadata_serializer_ptr;
  kva::image_io_sptr image_writer;
  auto config = this->find_configuration( "applets/dump_klv.conf" );

  // If --config given, read in config file, merge in with default just
  // generated
  if( cmd_args.count( "config" ) )
  {
    config->merge_config(
      kv::read_config_file(
        cmd_args[ "config" ].as< std::string >() ) );
  }

  // Output file extension configures exporter
  if( cmd_args.count( "log" ) &&
      !cmd_args.count( "metadata_serializer:type" ) &&
      !cmd_args.count( "exporter" ) )
  {
    const std::string filename = cmd_args[ "log" ].as< std::string >();
    auto const extension_pos = filename.rfind( '.' );
    if( extension_pos != filename.npos )
    {
      static std::map< std::string, std::string > const extension_map = {
        { ".JSON", "json" },
        { ".json", "json" },
        { ".CSV", "csv" },
        { ".csv", "csv" }, };
      auto const extension = filename.substr( extension_pos );
      auto const it = extension_map.find( extension );
      auto const serializer_type =
        ( it != extension_map.end() ) ? it->second : std::string{ "csv" };
      config->set_value( "metadata_serializer:type", serializer_type );
    }
  }

  if( cmd_args.count( "exporter" ) )
  {
    auto const serializer_type = cmd_args[ "exporter" ].as< std::string >();
    config->set_value( "metadata_serializer:type", serializer_type );
  }

  if( cmd_args.count( "compress" ) )
  {
    config->set_value( "metadata_serializer:klv-json:compress", true );
  }

  kv::set_nested_algo_configuration<>( "video_reader", config, video_reader );
  kv::get_nested_algo_configuration<>( "video_reader", config, video_reader );

  kv::set_nested_algo_configuration<>(
    "metadata_serializer", config, metadata_serializer_ptr );
  kv::get_nested_algo_configuration<>(
    "metadata_serializer", config, metadata_serializer_ptr );

  if( cmd_args.count( "frames" ) )
  {
    kv::set_nested_algo_configuration(
      "image_writer", config, image_writer );
    kv::get_nested_algo_configuration(
      "image_writer", config, image_writer );
  }

  // Check to see if we are to dump config
  if( cmd_args.count( "output" ) )
  {
    const std::string out_file = cmd_args[ "output" ].as< std::string >();
    std::ofstream fout( out_file.c_str() );
    if( !fout )
    {
      std::cout << "Couldn't open \"" << out_file << "\" for writing.\n";
      return EXIT_FAILURE;
    }

    kv::config_block_formatter fmt( config );
    fmt.print( fout );
    std::cout << "Wrote config to \"" << out_file << "\". Exiting.\n";
    return EXIT_SUCCESS;
  }

  if( !kv::check_nested_algo_configuration< kva::video_input >(
    "video_reader", config ) )
  {
    std::cerr << "Invalid video_reader config" << std::endl;
    return EXIT_FAILURE;
  }

  if( !kv::check_nested_algo_configuration< kva::metadata_map_io >(
    "metadata_serializer", config ) )
  {
    std::cerr << "Invalid metadata_serializer config" << std::endl;
    return EXIT_FAILURE;
  }

  if( cmd_args.count( "frames" ) &&
      !kv::check_nested_algo_configuration< kva::image_io >(
        "image_writer", config ) )
  {
    std::cerr << "Invalid image_writer config" << std::endl;
    return EXIT_FAILURE;
  }

  // Check if there's a frame range restriction
  kv::frame_id_t frame_begin = 1;
  kv::frame_id_t frame_end = std::numeric_limits< kv::frame_id_t >::max();
  if( cmd_args.count( "frame-range" ) )
  {
    auto const string = cmd_args[ "frame-range" ].as< std::string >();
    auto const pattern = std::regex{ "([0-9]+)|([0-9]+)?-([0-9]+)?" };
    std::smatch match;
    if( !std::regex_match( string, match, pattern ) )
    {
      std::cerr << "Invalid argument to --frame-range" << std::endl;
      return EXIT_FAILURE;
    }

    if( match[ 1 ].matched )
    {
      // Single frame
      frame_begin = std::stoi( match[ 1 ].str() );
      frame_end = frame_begin + 1;
    }
    else
    {
      // Multi-frame
      if( match[ 2 ].matched )
      {
        frame_begin = std::stoi( match[ 2 ].str() );
      }

      if( match[ 3 ].matched )
      {
        frame_end = std::stoi( match[ 3 ].str() );
      }
    }
  }

  // instantiate a video reader
  try
  {
    video_reader->open( video_file );
  }
  catch( kv::video_exception const& e )
  {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  catch( kv::file_not_found_exception const& e )
  {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  kv::timestamp ts;
  kv::wrap_text_block wtb;
  kv::metadata_map::map_metadata_t frame_metadata;
  std::vector< std::future< void > > image_write_futures;

  wtb.set_indent_string( "    " );

  // Avoid repeated dictionary access
  auto const detail = cmd_args[ "detail" ].as< bool >();
  auto const quiet = cmd_args[ "quiet" ].as< bool >();
  auto const log = ( cmd_args.count( "log" ) > 0 );

  while( video_reader->next_frame( ts ) )
  {
    if( ts.get_frame() < frame_begin )
    {
      continue;
    }

    if( ts.get_frame() >= frame_end )
    {
      break;
    }

    if( !quiet )
    {
      std::cout
        << "Frame # " << std::setw( 6 ) << std::setfill( ' ' ) << ts.get_frame()
        << " @ " << std::fixed << std::setprecision( 6 )
        << ts.get_time_seconds() << " sec" << std::endl
        << std::string( 64, '-' ) << std::endl;
    }

    kv::metadata_vector metadata = video_reader->frame_metadata();

    if( log )
    {
      // Add the (frame number, vector of metadata packets) item
      frame_metadata.insert( { ts.get_frame(), metadata } );
    }

    if( !quiet )
    {
      size_t packet_count = 1;
      for( auto const& meta : metadata )
      {
        std::cout
          << "Metadata packet #" << packet_count << std::endl
          << std::string( 32, '-' ) << std::endl;

        if( detail )
        {
          for( auto const& entry : *meta )
          {
            auto const description =
              kv::tag_traits_by_tag( entry.first ).description();
            auto const value_string =
              kv::metadata::format_string( entry.second->as_string() );

            std::cout
              << entry.second->name() << std::endl
              << wtb.wrap_text( description )
              << "Type:  " << entry.second->type_name() << std::endl
              << "Value: " << value_string << std::endl << std::endl;
          }
        }
        else
        {
          print_metadata( std::cout, *meta );
        }
        std::cout << std::endl;
        ++packet_count;
      }

      std::cout << std::endl;
    }

    if( cmd_args.count( "frames" ) )
    {
      std::string directory = ".";
      if( cmd_args.count( "frames-dir" ) )
      {
        directory = cmd_args[ "frames-dir" ].as< std::string >();
      }

      auto const name = kv::basename_from_metadata( metadata, ts.get_frame() );
      auto const extension = cmd_args[ "frames" ].as< std::string >();
      auto const filename = name + "." + extension;
      auto const filepath =
        kwiversys::SystemTools::JoinPath( { "", directory, filename } );
      auto const image = video_reader->frame_image();
      auto const task = [ image_writer, image, filepath ](){
                          image_writer->save( filepath, image );
                        };
      if( cmd_args[ "multithread" ].as< bool >() )
      {
        image_write_futures.emplace_back(
          vital::thread_pool::instance().enqueue( task ) );
      }
      else
      {
        task();
      }
    }
  }

  if( log )
  {
    const std::string out_file = cmd_args[ "log" ].as< std::string >();
    std::ofstream fout( out_file.c_str() );
    if( !fout )
    {
      std::cout << "Couldn't open \"" << out_file << "\" for writing.\n";
      return EXIT_FAILURE;
    }

    kv::metadata_map_sptr mms = std::make_shared< kv::simple_metadata_map >(
      kv::simple_metadata_map( frame_metadata ) );

    metadata_serializer_ptr->save( out_file, mms );

    std::cout << "Wrote KLV log to \"" << out_file << "\".\n";
  }

  for( auto& future : image_write_futures )
  {
    future.wait();
  }

  std::cout << "-- End of video --\n";

  return EXIT_SUCCESS;
}

} // namespace core

} // namespace arrows

} // namespace kwiver
