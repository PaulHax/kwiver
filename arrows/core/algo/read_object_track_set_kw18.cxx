// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Implementation of read_object_track_set_kw18

#include "read_object_track_set_kw18.h"

#include <vital/util/data_stream_reader.h>
#include <vital/util/tokenize.h>
#include <vital/vital_config.h>

namespace kwiver {

namespace arrows {

namespace core {

// field numbers for KW18 file format
enum
{
  COL_ID = 0,   // 0: Object ID
  COL_LEN,      // 1: Track length (always 1 for detections)
  COL_FRAME,    // 2: in this case, set index
  COL_LOC_X,    // 3
  COL_LOC_Y,    // 4
  COL_VEL_X,    // 5
  COL_VEL_Y,    // 6
  COL_IMG_LOC_X, // 7
  COL_IMG_LOC_Y, // 8
  COL_MIN_X,    // 9
  COL_MIN_Y,    // 10
  COL_MAX_X,    // 11
  COL_MAX_Y,    // 12
  COL_AREA,     // 13
  COL_WORLD_X,  // 14
  COL_WORLD_Y,  // 15
  COL_WORLD_Z,  // 16
  COL_TIME,     // 17
  COL_CONFIDENCE, // 18
};

// ----------------------------------------------------------------------------
class read_object_track_set_kw18::priv
{
public:
  priv( read_object_track_set_kw18& parent )
    : parent( parent ),
      m_first( true ),
      m_current_idx( 0 ),
      m_last_idx( 1 )
  {}

  read_object_track_set_kw18& parent;

  // Configuration values
  bool c_batch_load() { return parent.c_batch_load; }
  std::string c_delim() { return parent.c_delim; }

  ~priv() {}

  // Local values
  bool m_first;

  vital::frame_id_t m_current_idx;
  vital::frame_id_t m_last_idx;

  void read_all();

  // Map of object tracks indexed by frame number. Each set contains all tracks
  // referenced (active) on that individual frame.
  std::map< vital::frame_id_t,
    std::vector< vital::track_sptr > > m_tracks_by_frame_id;

  // Compilation of all loaded tracks, track id -> track sptr mapping
  std::map< vital::frame_id_t, vital::track_sptr > m_all_tracks;
};

// ----------------------------------------------------------------------------
void
read_object_track_set_kw18
::initialize()
{
  KWIVER_INITIALIZE_UNIQUE_PTR( priv, d );
  attach_logger( "arrows.core.read_object_track_set_kw18" );
}

read_object_track_set_kw18
::~read_object_track_set_kw18()
{}

// ----------------------------------------------------------------------------
bool
read_object_track_set_kw18
::check_configuration( VITAL_UNUSED vital::config_block_sptr config ) const
{
  return true;
}

// ----------------------------------------------------------------------------
bool
read_object_track_set_kw18
::read_set( vital::object_track_set_sptr& set )
{
  auto const first = d->m_first;
  if( first )
  {
    // Read in all detections
    d->read_all();
    d->m_first = false;
  }

  if( d->c_batch_load() )
  {
    if( !first )
    {
      return false;
    }

    std::vector< vital::track_sptr > trks;

    for( auto const& it : d->m_all_tracks )
    {
      trks.push_back( it.second );
    }

    set = std::make_shared< vital::object_track_set >( trks );
    return true;
  }

  // Return detection set at current index if there is one
  if( d->m_tracks_by_frame_id.count( d->m_current_idx ) == 0 )
  {
    // Return empty set
    set = std::make_shared< vital::object_track_set >();
  }
  else
  {
    // Return tracks for this frame
    vital::object_track_set_sptr new_set(
      new vital::object_track_set(
        d->m_tracks_by_frame_id[ d->m_current_idx ] ) );

    set = new_set;
  }

  ++d->m_current_idx;

  // Return if we are done parsing
  return this->at_eof();
}

// ----------------------------------------------------------------------------
void
read_object_track_set_kw18::priv
::read_all()
{
  std::string line;
  vital::data_stream_reader stream_reader( parent.stream() );

  m_tracks_by_frame_id.clear();
  m_all_tracks.clear();

  while( stream_reader.getline( line ) )
  {
    if( !line.empty() && line[ 0 ] == '#' )
    {
      continue;
    }

    std::vector< std::string > col;
    vital::tokenize( line, col, c_delim(), true );

    if( ( col.size() < 18 ) || ( col.size() > 20 ) )
    {
      std::stringstream str;

      str << "This is not a kw18 kw19 or kw20 file; found "
          << col.size() << " columns in\n\"" << line << "\"";

      VITAL_THROW( vital::invalid_data, str.str() );
    }

    //  Check to see if we have seen this frame before. If we have,
    //  then retrieve the frame's index into our output map. If not
    //  seen before, add frame -> detection set index to our map and
    //  press on.
    //
    //  This allows for track states to be written in a non-contiguous
    //  manner as may be done by streaming writers.
    vital::frame_id_t frame_index = atoi( col[ COL_FRAME ].c_str() );
    vital::time_usec_t frame_time = atof( col[ COL_TIME ].c_str() );
    int track_index = atoi( col[ COL_ID ].c_str() );

    vital::bounding_box_d bbox(
      atof( col[ COL_MIN_X ].c_str() ),
      atof( col[ COL_MIN_Y ].c_str() ),
      atof( col[ COL_MAX_X ].c_str() ),
      atof( col[ COL_MAX_Y ].c_str() ) );

    double conf = 1.0;

    if( col.size() == 19 )
    {
      conf = atof( col[ COL_CONFIDENCE ].c_str() );
    }

    // Create new detection
    vital::detected_object_sptr det =
      std::make_shared< vital::detected_object >( bbox, conf );

    // Create new object track state
    vital::track_state_sptr ots =
      std::make_shared< vital::object_track_state >(
        frame_index, frame_time,
        det );

    // Assign object track state to track
    vital::track_sptr trk;

    if( m_all_tracks.count( track_index ) == 0 )
    {
      trk = vital::track::create();
      trk->set_id( track_index );
      m_all_tracks[ track_index ] = trk;
    }
    else
    {
      trk = m_all_tracks[ track_index ];
    }

    trk->append( ots );

    // Add track to indexes
    if( !c_batch_load() )
    {
      m_tracks_by_frame_id[ frame_index ].push_back( trk );
      m_last_idx = std::max( m_last_idx, frame_index );
    }
  }
}

} // namespace core

} // namespace arrows

}     // end namespace
