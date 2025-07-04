// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include "track_scorable_mgrs.h"

#include <sstream>
#include <stdexcept>

#include <track_oracle/track_kw18/track_kw18.h>
#ifdef SHAPELIB_ENABLED
#include <track_oracle/track_apix/track_apix.h>
#endif
#include <track_oracle/file_format_schema.h>
#include <track_oracle/file_format_type.h>

#include <vital/logger/logger.h>
static kwiver::vital::logger_handle_t main_logger( kwiver::vital::get_logger(
  __FILE__ ) );

using std::pair;
using std::string;

namespace kwiver {

namespace track_oracle {

//
// Attempt to set the scorable_mgrs fields for the frames
// in the tracks in the given track list.  Return false if
// any of the frames fail to yield a geocoord; true otherwise.
//

bool
track_scorable_mgrs_type
::set_from_tracklist(
  const track_handle_list_type& tracks,
  const string& lon_field_name,
  const string& lat_field_name )
{
  file_format_schema_type ffs;

#ifdef SHAPELIB_ENABLED
  track_apix_type apix_schema;
#endif
  track_kw18_type kw18_schema;
  track_scorable_mgrs_type mgrs_schema;
  track_field< double > lat_field( lat_field_name );
  track_field< double > lon_field( lon_field_name );

  bool okay = true;
  bool first_time = true;

  for( size_t i = 0; i < tracks.size(); ++i )
  {
    const track_handle_type& t = tracks[ i ];
    const frame_handle_list_type& frames = track_oracle_core::get_frames( t );

    file_format_enum tf = ffs( t ).format();

    double lat, lon;
    for( size_t f_index = 0; f_index < frames.size(); ++f_index )
    {
      const frame_handle_type& f = frames[ f_index ];
      switch( tf )
      {
#ifdef SHAPELIB_ENABLED
        case TF_APIX:
          lat = apix_schema[ f ].lat();
          lon = apix_schema[ f ].lon();
          break;
#endif

        case TF_KW18:
          lat = kw18_schema[ f ].world_y();
          lon = kw18_schema[ f ].world_x();
          break;

        default:
          // okay, it's not an APIX or a KW18 track, so we'll try to get the
          // lat/lon
          // out of track fields the user named
          pair< bool, double > lat_probe = lat_field.get( f.row );
          pair< bool, double > lon_probe = lon_field.get( f.row );
          if( !( lat_probe.first && lon_probe.first ) )
          {
            LOG_ERROR(
              main_logger,
              "Couldn't find latitude and/or longitude field to set MGRS (radial overlap) info-- format is "
                << file_format_type::to_string( tf ) );
            return false;
          }
          lat = lat_probe.second;
          lon = lon_probe.second;
          if( first_time )
          {
            LOG_INFO(
              main_logger,
              "Using " << file_format_type::to_string( tf ) << " fields '" <<
                lon_field_name
                       << "' for longitude (" << lon << "), '" <<
                lat_field_name << "' for latitude (" << lat << ")" );
            first_time = false;
          }
          break;
      }

      mgrs_schema[ f ].mgrs() = scorable_mgrs(
        geographic::geo_coords(
          lat,
          lon ) );
    } // ...for each frame;
  } // ...for each track

  return okay;
}

} // ...track_oracle

} // ...kwiver
