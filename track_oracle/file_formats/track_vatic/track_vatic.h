// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#ifndef INCL_TRACK_VATIC_H
#define INCL_TRACK_VATIC_H

#include <track_oracle/data_terms/data_terms.h>
#include <track_oracle/file_formats/track_vatic/track_vatic_export.h>
#include <vital/vital_config.h>

#include <set>
#include <track_oracle/core/track_base.h>
#include <track_oracle/core/track_field.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_point_2d.h>

namespace kwiver {

namespace track_oracle {

struct TRACK_VATIC_EXPORT track_vatic_type : public track_base< track_vatic_type >
{
  // track level data
  track_field< dt::tracking::external_id > external_id;

  // frame level data
  track_field< dt::tracking::bounding_box > bounding_box;
  track_field< dt::tracking::frame_number > frame_number;
  track_field< bool >& lost;
  track_field< bool >& occluded;
  track_field< bool >& generated;
  track_field< std::string >& label;
  track_field< std::set< std::string > >& attributes;

  track_vatic_type()
    : lost( Frame.add_field< bool >( "lost" ) ),
      occluded( Frame.add_field< bool >( "occluded" ) ),
      generated( Frame.add_field< bool >( "generated" ) ),
      label( Frame.add_field< std::string >( "label" ) ),
      attributes( Frame.add_field< std::set< std::string > >(
        "vatic_attributes" ) )
  {
    Track.add_field( external_id );
    Frame.add_field( bounding_box );
    Frame.add_field( frame_number );
  }
};

} // ...track_oracle

} // ...kwiver

#endif
