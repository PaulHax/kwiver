// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include "video_input_filter.h"

#include <vital/algo/algorithm.h>
#include <vital/algo/algorithm.txx>
#include <vital/exceptions.h>
#include <vital/types/timestamp.h>
#include <vital/vital_types.h>

#include <vector>

namespace kwiver {

namespace arrows {

namespace core {

class video_input_filter::priv
{
public:
  priv( video_input_filter& parent )
    : parent( parent ),
      d_at_eov( false )
  {}

  video_input_filter& parent;
  // Configuration values
  vital::frame_id_t c_start_at_frame() { return parent.c_start_at_frame; }
  vital::frame_id_t c_stop_after_frame() { return parent.c_stop_after_frame; }
  vital::frame_id_t c_frame_skip() { return parent.c_output_nth_frame; }
  double c_frame_rate() { return parent.c_frame_rate; }

  // local state
  bool d_at_eov;

  // processing classes
  vital::algo::video_input_sptr d_video_input() { return parent.c_video_input; }
};

// ----------------------------------------------------------------------------
void
video_input_filter
::initialize()
{
  KWIVER_INITIALIZE_UNIQUE_PTR( priv, d );
  attach_logger( "arrows.core.video_input_filter" );
}

// ----------------------------------------------------------------------------
video_input_filter
::~video_input_filter()
{}

// ----------------------------------------------------------------------------
bool
video_input_filter
::check_configuration( vital::config_block_sptr config ) const
{
  bool retcode = true;

  // Validate start frame
  if( config->has_value( "start_at_frame" ) )
  {
    //  zero indicates not set, otherwise must be 1 or greater
    if( config->get_value< vital::frame_id_t >( "start_at_frame" ) < 0 )
    {
      LOG_ERROR(logger(), "start_at_frame must be non-negative" );
      retcode = false;
    }
  }

  // Validate stop frame
  if( config->has_value( "stop_after_frame" ) )
  {
    //  zero indicates not set, otherwise must be 1 or greater
    if( config->get_value< vital::frame_id_t >( "stop_after_frame" ) < 0 )
    {
      LOG_ERROR(logger(), "stop_after_frame must be non-negative" );
      retcode = false;
    }
  }

  // Make sure start frame is not after stop frame
  if( config->has_value( "start_at_frame" ) &&
      config->has_value( "stop_after_frame" ) )
  {
    if( config->get_value< vital::frame_id_t >( "stop_after_frame" ) > 0 &&
        config->get_value< vital::frame_id_t >( "start_at_frame" ) > 0 &&
        config->get_value< vital::frame_id_t >( "stop_after_frame" ) <
        config->get_value< vital::frame_id_t >( "start_at_frame" ) )
    {
      LOG_ERROR(
        logger(),
        "stop_after_frame must not be before start_at_frame" );
      retcode = false;
    }
  }

  // Validate skip frames
  if( config->has_value( "output_nth_frame" ) )
  {
    // Must be a positve integer
    if( config->get_value< vital::frame_id_t >( "output_nth_frame" ) <= 0 )
    {
      LOG_ERROR(logger(), "output_nth_frame must be greater than 0" );
      retcode = false;
    }
  }

  // Check the video input configuration.
  return retcode &&
         vital::check_nested_algo_configuration< vital::algo::video_input >(
    "video_input", config );
}

// ----------------------------------------------------------------------------
void
video_input_filter
::open( std::string name )
{
  if( !d->d_video_input() )
  {
    // VITAL_THROW( kwiver::vital::algorithm_configuration_exception,
    // type_name(), impl_name(),
    //      "invalid video_input." );
    VITAL_THROW(
      kwiver::vital::algorithm_configuration_exception,
      this->interface_name(), this->plugin_name(), "invalid video_input." );
  }
  d->d_video_input()->open( name );
  d->d_at_eov = false;

  auto const& vi_caps = d->d_video_input()->get_implementation_capabilities();

  typedef vital::algo::video_input vi;

  // pass through capabilities, modified as needed
  set_capability(
    vi::HAS_EOV,
    vi_caps.capability( vi::HAS_EOV ) ||
    this->c_stop_after_frame > 0 );
  set_capability(
    vi::HAS_FRAME_NUMBERS,
    vi_caps.capability( vi::HAS_FRAME_NUMBERS ) );
  set_capability(
    vi::HAS_FRAME_DATA,
    vi_caps.capability( vi::HAS_FRAME_DATA ) );
  set_capability(
    vi::HAS_FRAME_TIME,
    vi_caps.capability( vi::HAS_FRAME_TIME ) ||
    this->c_frame_rate > 0 );
  set_capability(
    vi::HAS_METADATA,
    vi_caps.capability( vi::HAS_METADATA ) );
  set_capability(
    vi::HAS_ABSOLUTE_FRAME_TIME,
    vi_caps.capability( vi::HAS_ABSOLUTE_FRAME_TIME ) );
  set_capability(
    vi::HAS_TIMEOUT,
    vi_caps.capability( vi::HAS_TIMEOUT ) );
  set_capability(
    vi::IS_SEEKABLE,
    vi_caps.capability( vi::IS_SEEKABLE ) );
}

// ----------------------------------------------------------------------------
void
video_input_filter
::close()
{
  if( d->d_video_input() )
  {
    d->d_video_input()->close();
  }
}

// ----------------------------------------------------------------------------
bool
video_input_filter
::end_of_video() const
{
  return d->d_at_eov;
}

// ----------------------------------------------------------------------------
bool
video_input_filter
::good() const
{
  if( !d->d_video_input() )
  {
    return false;
  }
  return d->d_video_input()->good();
}

// ----------------------------------------------------------------------------
bool
video_input_filter
::seekable() const
{
  if( !d->d_video_input() )
  {
    return false;
  }
  return d->d_video_input()->seekable();
}

// ----------------------------------------------------------------------------
size_t
video_input_filter
::num_frames() const
{
  if( !d->d_video_input() )
  {
    return 0;
  }
  if( d->c_stop_after_frame() > 0 )
  {
    return ( size_t ) std::min(
      static_cast< vital::timestamp::frame_t >( d->d_video_input()->num_frames() ),
      d->c_stop_after_frame() ) - ( d->c_start_at_frame() - 1 );
  }
  else
  {
    return d->d_video_input()->num_frames() - ( d->c_start_at_frame() - 1 );
  }
}

// ----------------------------------------------------------------------------
bool
video_input_filter
::next_frame(
  kwiver::vital::timestamp& ts,               // returns timestamp
  uint32_t timeout )                              // not supported
{
  // Check for at end of data
  if( d->d_at_eov )
  {
    return false;
  }

  bool status = false;

  do
  {
    status = d->d_video_input()->next_frame( ts, timeout );

    if( !status )
    {
      d->d_at_eov = d->d_video_input()->end_of_video();
      return false;
    }

    if( d->c_stop_after_frame() > 0 &&
        ts.get_frame() > d->c_stop_after_frame() )
    {
      d->d_at_eov = true;
      return false;
    }
  }while( ( ts.get_frame() - 1 ) % d->c_frame_skip() != 0 ||
          ts.get_frame() < d->c_start_at_frame() );

  // set the frame time base on rate if missing
  if( d->c_frame_rate() > 0 && !ts.has_valid_time() )
  {
    ts.set_time_seconds( ts.get_frame() / d->c_frame_rate() );
  }

  return status;
}

// ----------------------------------------------------------------------------
bool
video_input_filter
::seek_frame(
  kwiver::vital::timestamp& ts,               // returns timestamp
  kwiver::vital::timestamp::frame_t frame_number,
  uint32_t timeout )                              // not supported
{
  // Check if requested frame is valid
  if( ( d->c_stop_after_frame() != 0 &&
        d->c_stop_after_frame() < frame_number ) ||
      frame_number < d->c_start_at_frame() ||
      ( frame_number - 1 ) % d->c_frame_skip() != 0 )
  {
    return false;
  }

  bool status = d->d_video_input()->seek_frame( ts, frame_number, timeout );

  // set the frame time base on rate if missing
  if( d->c_frame_rate() > 0 && !ts.has_valid_time() )
  {
    ts.set_time_seconds( ts.get_frame() / d->c_frame_rate() );
  }

  return status;
}

// ----------------------------------------------------------------------------
kwiver::vital::timestamp
video_input_filter
::frame_timestamp() const
{
  // Check for at end of data
  if( d->d_at_eov )
  {
    return {};
  }

  auto ts = d->d_video_input()->frame_timestamp();

  // set the frame time base on rate if missing
  if( d->c_frame_rate() > 0 && ts.has_valid_frame() && !ts.has_valid_time() )
  {
    ts.set_time_seconds( ts.get_frame() / d->c_frame_rate() );
  }

  return ts;
}

// ----------------------------------------------------------------------------
kwiver::vital::image_container_sptr
video_input_filter
::frame_image()
{
  if( !this->end_of_video() )
  {
    return d->d_video_input()->frame_image();
  }
  return nullptr;
}

// ----------------------------------------------------------------------------
kwiver::vital::video_raw_metadata_sptr
video_input_filter
::raw_frame_metadata()
{
  if( !d->d_video_input() )
  {
    return nullptr;
  }
  return d->d_video_input()->raw_frame_metadata();
}

// ----------------------------------------------------------------------------
kwiver::vital::metadata_vector
video_input_filter
::frame_metadata()
{
  if( !this->end_of_video() )
  {
    return d->d_video_input()->frame_metadata();
  }
  return kwiver::vital::metadata_vector();
}

kwiver::vital::metadata_map_sptr
video_input_filter
::metadata_map()
{
  vital::metadata_map::map_metadata_t output_map;

  auto internal_map = d->d_video_input()->metadata_map()->metadata();
  auto start = internal_map.find( d->c_start_at_frame() );
  auto stop = internal_map.find( d->c_stop_after_frame() );
  if( stop != internal_map.end() )
  {
    stop++; // stop frame should be included
  }

  if( d->c_frame_skip() == 1 )
  {
    if( d->c_stop_after_frame() > 0 )
    {
      output_map.insert( start, stop );
    }
    else
    {
      output_map.insert( start, internal_map.end() );
    }
  }
  else
  {
    for( auto it = start; it != stop; ++it )
    {
      if( ( it->first - 1 ) % d->c_frame_skip() == 0 )
      {
        output_map.insert( *it );
      }
    }
  }

  return std::make_shared< kwiver::vital::simple_metadata_map >( output_map );
}

// ----------------------------------------------------------------------------
kwiver::vital::video_settings_uptr
video_input_filter
::implementation_settings() const
{
  return d->d_video_input()->implementation_settings();
}

} // namespace core

} // namespace arrows

}         // end namespace
