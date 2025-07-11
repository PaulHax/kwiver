// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Implementation of detected object set csv output

#include "write_object_track_set_kw18.h"

#include <vital/vital_config.h>

#include <time.h>

namespace kwiver {

namespace arrows {

namespace core {

/// This format should only be used for tracks.
///
/// \li Column(s) 1: Track-id
/// \li Column(s) 2: Track-length (# of detections)
/// \li Column(s) 3: Frame-number (-1 if not available)
/// \li Column(s) 4-5: Tracking-plane-loc(x,y) (Could be same as World-loc)
/// \li Column(s) 6-7: Velocity(x,y)
/// \li Column(s) 8-9: Image-loc(x,y)
/// \li Column(s) 10-13: Img-bbox(TL_x,TL_y,BR_x,BR_y)
/// \li Column(s) 14: Area (0 - when not available)
/// \li Column(s) 15-17: World-loc(x,y,z) (long, lat, 0 - when not available)
/// \li Column(s) 18: Timesetamp(-1 if not available)
/// \li Column(s) 19: Track-confidence(-1_when_not_available)

// ----------------------------------------------------------------------------
class write_object_track_set_kw18::priv
{
public:
  priv( write_object_track_set_kw18& parent )
    : parent( parent ),
      m_first( true ),
      m_frame_number( 1 )
  {}

  ~priv() {}

  write_object_track_set_kw18& parent;

  // Configuration values
  std::string c_delim() { return parent.c_delim; }

  // Local values
  bool m_first;
  int m_frame_number;
  std::string m_delim;
  std::map< unsigned, vital::track_sptr > m_tracks;
};

// ----------------------------------------------------------------------------
void
write_object_track_set_kw18
::initialize()
{
  KWIVER_INITIALIZE_UNIQUE_PTR( priv, d );
  attach_logger( "arrows.core.write_object_track_set_kw18" );
}

write_object_track_set_kw18
::~write_object_track_set_kw18()
{}

void
write_object_track_set_kw18
::close()
{
  for( auto trk_pair : d->m_tracks )
  {
    auto trk_ptr = trk_pair.second;

    for( auto ts_ptr : *trk_ptr )
    {
      vital::object_track_state* ts =
        dynamic_cast< vital::object_track_state* >( ts_ptr.get() );

      if( !ts )
      {
        LOG_ERROR(
          d->parent.logger(),
          "MISSED STATE " << trk_ptr->id() << " " << trk_ptr->size() );
        continue;
      }

      vital::detected_object_sptr det = ts->detection();
      const vital::bounding_box_d empty_box = vital::bounding_box_d(
        -1, -1, -1,
        -1 );
      vital::bounding_box_d bbox = ( det ? det->bounding_box() : empty_box );

      stream() << trk_ptr->id() << " "     // 1: track id
               << trk_ptr->size() << " "   // 2: track length
               << ts->frame() << " "       // 3: frame number
               << "0 "                     // 4: tracking plane x
               << "0 "                     // 5: tracking plane y
               << "0 "                     // 6: velocity x
               << "0 "                     // 7: velocity y
               << bbox.center()[ 0 ] << " " // 8: image location x
               << bbox.center()[ 1 ] << " " // 9: image location y
               << bbox.min_x() << " "      // 10: TL-x
               << bbox.min_y() << " "      // 11: TL-y
               << bbox.max_x() << " "      // 12: BR-x
               << bbox.max_y() << " "      // 13: BR-y
               << bbox.area() << " "       // 14: area
               << "0 "                     // 15: world-loc x
               << "0 "                     // 16: world-loc y
               << "0 "                     // 17: world-loc z
               << ts->time() << " "        // 18: timestamp
               << det->confidence()        // 19: confidence
               << std::endl;
    }
  }

  write_object_track_set::close();
}

// ----------------------------------------------------------------------------
bool
write_object_track_set_kw18
::check_configuration( VITAL_UNUSED vital::config_block_sptr config ) const
{
  return true;
}

// ----------------------------------------------------------------------------
void
write_object_track_set_kw18
::write_set(
  kwiver::vital::object_track_set_sptr const& set,
  kwiver::vital::timestamp const& /*ts*/,
  std::string const& /*frame_identifier*/ )
{
  if( d->m_first )
  {
    // Write file header(s)
    stream() << "# 1:Track-id "
             << "2:Track-length "
             << "3:Frame-number "
             << "4:Tracking-plane-loc(x) "
             << "5:Tracking-plane-loc(y) "
             << "6:velocity(x) "
             << "7:velocity(y) "

             << "8:Image-loc(x) "
             << "9:Image-loc(y) "
             << "10:Img-bbox(TL_x) "
             << "11:Img-bbox(TL_y) "
             << "12:Img-bbox(BR_x) "
             << "13:Img-bbox(BR_y) "
             << "14:Area "

             << "15:World-loc(x) "
             << "16:World-loc(y) "
             << "17:World-loc(z) "
             << "18:timestamp "
             << "19:track-confidence"
             << std::endl;

    d->m_first = false;
  }

  for( auto trk : set->tracks() )
  {
    d->m_tracks[ trk->id() ] = trk;
  }
}

} // namespace core

} // namespace arrows

}     // end namespace
