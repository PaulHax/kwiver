// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#ifndef INCL_TRACK_SCORABLE_MGRS_H
#define INCL_TRACK_SCORABLE_MGRS_H

///
/// This class supplies both the track fields and the adapter methods
/// to populate the scorable_mgrs from existing tracks (i.e. kw18 with world
/// coords or apix tracks.)  This is required since the scorable_mgrs
/// type isn't natively found in any file format recognized by the top-level
/// generic track reader.
///

#include <track_oracle/track_scorable_mgrs/track_scorable_mgrs_export.h>
#include <vital/vital_config.h>

#include <track_oracle/track_base.h>
#include <track_oracle/track_field.h>
#include <track_oracle/track_scorable_mgrs/scorable_mgrs_data_term.h>

namespace kwiver {

namespace track_oracle {

struct TRACK_SCORABLE_MGRS_EXPORT
track_scorable_mgrs_type : public track_base< track_scorable_mgrs_type >
{
  track_field< dt::tracking::mgrs_pos > mgrs;

  track_scorable_mgrs_type()
  {
    Frame.add_field( mgrs );
  }

  static bool set_from_tracklist(
    const track_handle_list_type& tracks,
    const std::string& lon_field_name = "longitude",
    const std::string& lat_field_name = "latitude" );
};

} // ...track_oracle

} // ...kwiver

#endif
