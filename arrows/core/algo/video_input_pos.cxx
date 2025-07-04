// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include "video_input_pos.h"

#include <vital/exceptions.h>
#include <vital/io/metadata_io.h>
#include <vital/types/metadata.h>
#include <vital/types/metadata_traits.h>
#include <vital/types/timestamp.h>
#include <vital/util/data_stream_reader.h>
#include <vital/vital_config.h>
#include <vital/vital_types.h>

#include <kwiversys/SystemTools.hxx>

#include <fstream>

namespace kwiver {

namespace arrows {

namespace core {

class video_input_pos::priv
{
public:
  priv( video_input_pos& parent )
    : m_parent( parent ),
      d_current_files( d_img_md_files.end() ),
      d_frame_number( 0 ),
      d_metadata( nullptr ),
      d_have_metadata_map( false )
  {}

  video_input_pos& m_parent;

  // Configuration values
  std::string c_meta_directory() { return m_parent.get_metadata_directory(); }
  std::string c_meta_extension() { return m_parent.get_metadata_extension(); }

  std::string c_image_list_file;

  // local state
  typedef std::pair< vital::path_t, vital::path_t > path_pair_t;

  std::vector< path_pair_t > d_img_md_files;
  std::vector< path_pair_t >::const_iterator d_current_files;
  kwiver::vital::frame_id_t d_frame_number;

  vital::metadata_sptr d_metadata;

  // metadata map
  bool d_have_metadata_map;
  vital::metadata_map::map_metadata_t d_metadata_map;

  // Read and process metadata from file
  vital::metadata_sptr
  process_metadata(
    path_pair_t paths,
    vital::timestamp::frame_t frame,
    vital::timestamp& ts )
  {
    vital::metadata_sptr metadata = nullptr;

    if( !paths.second.empty() )
    {
      // Open next file in the list
      metadata = vital::read_pos_file( paths.second );
    }

    // Include the path to the image
    if( metadata )
    {
      metadata->add< vital::VITAL_META_IMAGE_URI >( paths.first );
    }

    // Return timestamp
    ts.set_frame( frame );
    if( metadata )
    {
      if( auto& item = metadata->find( vital::VITAL_META_GPS_SEC ) )
      {
        double gps_sec = item.as_double();
        // TODO: also use gps_week and convert to UTC to get abosolute time
        // or subtract off first frame time to get time relative to start
        ts.set_time_seconds( gps_sec );
      }
      metadata->set_timestamp( ts );
    }

    return metadata;
  }
};

// ----------------------------------------------------------------------------
void
video_input_pos
::initialize()
{
  KWIVER_INITIALIZE_UNIQUE_PTR( priv, d );
  attach_logger( "arrows.core.video_input_pos" );

  set_capability( vital::algo::video_input::HAS_EOV, true );
  set_capability( vital::algo::video_input::HAS_FRAME_NUMBERS, true );
  set_capability( vital::algo::video_input::HAS_FRAME_TIME, true );
  set_capability( vital::algo::video_input::HAS_METADATA, true );

  set_capability( vital::algo::video_input::HAS_FRAME_DATA, false );

  // MAYBE
  set_capability( vital::algo::video_input::HAS_ABSOLUTE_FRAME_TIME, false );

  set_capability( vital::algo::video_input::HAS_TIMEOUT, false );
  set_capability( vital::algo::video_input::IS_SEEKABLE, true );
}

// ----------------------------------------------------------------------------
video_input_pos
::~video_input_pos()
{}

// ----------------------------------------------------------------------------
bool
video_input_pos
::check_configuration( VITAL_UNUSED vital::config_block_sptr config ) const
{
  return true;
}

// ----------------------------------------------------------------------------
void
video_input_pos
::open( std::string image_list_name )
{
  typedef kwiversys::SystemTools ST;

  // close the video in case already open
  this->close();

  // open file and read lines
  std::ifstream ifs( image_list_name.c_str() );
  if( !ifs )
  {
    VITAL_THROW(
      kwiver::vital::invalid_file, image_list_name,
      "Could not open file" );
  }

  kwiver::vital::data_stream_reader stream_reader( ifs );

  // verify and get file names in a list
  std::string line;
  while( stream_reader.getline( line ) )
  {
    // Get base name from file
    std::string resolved_file = d->c_meta_directory();
    resolved_file += "/" + ST::GetFilenameWithoutLastExtension( line ) +
                     d->c_meta_extension();
    if( !ST::FileExists( resolved_file ) )
    {
      LOG_DEBUG(
        logger(), "Could not find file " << resolved_file
                                         <<
          ". This frame will not have any metadata." );
      resolved_file.clear(); // indicate that the metadata file could not be
                             // found
    }

    d->d_img_md_files.push_back( std::make_pair( line, resolved_file ) );
  } // end while

  d->d_current_files = d->d_img_md_files.begin();
  d->d_frame_number = 0;
}

// ----------------------------------------------------------------------------
void
video_input_pos
::close()
{
  d->d_img_md_files.clear();
  d->d_current_files = d->d_img_md_files.end();
  d->d_frame_number = 0;
  d->d_metadata = nullptr;
}

// ----------------------------------------------------------------------------
bool
video_input_pos
::end_of_video() const
{
  return d->d_current_files == d->d_img_md_files.end();
}

// ----------------------------------------------------------------------------
bool
video_input_pos
::good() const
{
  return d->d_frame_number > 0 && !this->end_of_video();
}

// ----------------------------------------------------------------------------
bool
video_input_pos
::seekable() const
{
  return true;
}

// ----------------------------------------------------------------------------
size_t
video_input_pos
::num_frames() const
{
  return d->d_img_md_files.size();
}

// ----------------------------------------------------------------------------
bool
video_input_pos
::next_frame(
  kwiver::vital::timestamp& ts,               // returns timestamp
  VITAL_UNUSED uint32_t timeout )                 // not supported
{
  // reset current metadata packet and timestamp
  d->d_metadata = nullptr;
  ts = kwiver::vital::timestamp();

  // Check for at end of video
  if( this->end_of_video() )
  {
    return false;
  }

  // do not increment the iterator on the first call to next_frame()
  if( d->d_frame_number > 0 )
  {
    ++d->d_current_files;
  }
  ++d->d_frame_number;

  // Check for at end of video
  if( this->end_of_video() )
  {
    return false;
  }

  if( !d->d_current_files->second.empty() )
  {
    // Open next file in the list
    d->d_metadata = vital::read_pos_file( d->d_current_files->second );
  }

  // Return timestamp
  ts = this->frame_timestamp();

  // Include the path to the image
  if( d->d_metadata )
  {
    d->d_metadata->set_timestamp( ts );
    d->d_metadata->add< vital::VITAL_META_IMAGE_URI >(
      d->d_current_files->first );
  }

  return true;
}

// ----------------------------------------------------------------------------
bool
video_input_pos
::seek_frame(
  kwiver::vital::timestamp& ts,               // returns timestamp
  kwiver::vital::timestamp::frame_t frame_number,
  VITAL_UNUSED uint32_t timeout )
{
  // reset current metadata packet and timestamp
  d->d_metadata = nullptr;
  ts = kwiver::vital::timestamp();

  // Check if requested frame exists
  if( frame_number > static_cast< int >( d->d_img_md_files.size() ) ||
      frame_number <= 0 )
  {
    return false;
  }

  // Adjust frame number if this is the first call to seek_frame or next_frame
  if( d->d_frame_number == 0 )
  {
    d->d_frame_number = 1;
  }

  // Calculate distance to new frame
  kwiver::vital::timestamp::frame_t frame_diff =
    frame_number - d->d_frame_number;
  d->d_current_files += frame_diff;
  d->d_frame_number = frame_number;

  if( !d->d_current_files->second.empty() )
  {
    // Open next file in the list
    d->d_metadata = vital::read_pos_file( d->d_current_files->second );
  }

  // Return timestamp
  ts = this->frame_timestamp();

  // Include the path to the image
  if( d->d_metadata )
  {
    d->d_metadata->set_timestamp( ts );
    d->d_metadata->add< vital::VITAL_META_IMAGE_URI >(
      d->d_current_files->first );
  }

  return true;
}

// ----------------------------------------------------------------------------
kwiver::vital::timestamp
video_input_pos
::frame_timestamp() const
{
  // Check for at end of video
  if( this->end_of_video() )
  {
    return {};
  }

  kwiver::vital::timestamp ts;

  ts.set_frame( d->d_frame_number );
  if( d->d_metadata )
  {
    if( auto& item = d->d_metadata->find( vital::VITAL_META_GPS_SEC ) )
    {
      double gps_sec = item.as_double();
      // TODO: also use gps_week and convert to UTC to get abosolute time
      // or subtract off first frame time to get time relative to start
      ts.set_time_seconds( gps_sec );
    }
  }

  return ts;
}

// ----------------------------------------------------------------------------
kwiver::vital::image_container_sptr
video_input_pos
::frame_image()
{
  return nullptr;
}

// ----------------------------------------------------------------------------
kwiver::vital::metadata_vector
video_input_pos
::frame_metadata()
{
  kwiver::vital::metadata_vector vect;
  if( d->d_metadata )
  {
    vect.push_back( d->d_metadata );
  }

  return vect;
}

kwiver::vital::metadata_map_sptr
video_input_pos
::metadata_map()
{
  if( !d->d_have_metadata_map )
  {
    kwiver::vital::timestamp::frame_t fn = 0;
    for( const auto& f : d->d_img_md_files )
    {
      ++fn;

      kwiver::vital::timestamp ts;
      auto md = d->process_metadata( f, fn, ts );
      if( md )
      {
        vital::metadata_vector mdv( 1, md );
        std::pair< vital::timestamp::frame_t, vital::metadata_vector > el( fn,
          mdv );
        d->d_metadata_map.insert( el );
      }
    }

    d->d_have_metadata_map = true;
  }

  return std::make_shared< kwiver::vital::simple_metadata_map >(
    d->d_metadata_map );
}

} // namespace core

} // namespace arrows

}         // end namespace
