// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#ifndef ARROWS_SERIALILIZATION_PROTOBUF_CONVERT_PROTOBUF_H
#define ARROWS_SERIALILIZATION_PROTOBUF_CONVERT_PROTOBUF_H

#include <arrows/serialize/protobuf/kwiver_serialize_protobuf_export.h>

#include <vital/types/activity.h>
#include <vital/types/bounding_box.h>
#include <vital/types/image_container.h>
#include <vital/types/metadata.h>
#include <vital/types/object_track_set.h>
#include <vital/types/track.h>
#include <vital/types/track_set.h>

namespace kwiver {

namespace vital {

class detected_object;
class detected_object_set;
class geo_point;
class geo_polygon;
class object_track_state;
class polygon;
class timestamp;
class track_state;

} // namespace vital

}   // end namespace

namespace kwiver {

namespace protobuf {

class activity;
class activity_type;
class bounding_box;
class detected_object_type;
class detected_object;
class detected_object_set;
class geo_point;
class geo_polygon;
class image;
class metadata;
class metadata_vector;
class object_track_state;
class object_track_set;
class polygon;
class string;
class timestamp;
class track;
class track_set;
class track_state;

} // namespace protobuf

}   // end namespace

namespace kwiver {

namespace arrows {

namespace serialize {

namespace protobuf {

// ---- activity
KWIVER_SERIALIZE_PROTOBUF_EXPORT
void convert_protobuf(
  const ::kwiver::protobuf::activity& proto_act,
  ::kwiver::vital::activity&          act );

KWIVER_SERIALIZE_PROTOBUF_EXPORT
void convert_protobuf(
  const ::kwiver::vital::activity& act,
  ::kwiver::protobuf::activity&    proto_act );

// ---- activity_type
KWIVER_SERIALIZE_PROTOBUF_EXPORT
void convert_protobuf(
  const ::kwiver::vital::activity_type& at,
  ::kwiver::protobuf::activity_type&  proto_at );

KWIVER_SERIALIZE_PROTOBUF_EXPORT
void convert_protobuf(
  const ::kwiver::protobuf::activity_type&  proto_at,
  ::kwiver::vital::activity_type& at );

// ---- bounding_box
KWIVER_SERIALIZE_PROTOBUF_EXPORT
void convert_protobuf(
  const ::kwiver::protobuf::bounding_box&  proto_bbox,
  ::kwiver::vital::bounding_box_d&         bbox );

KWIVER_SERIALIZE_PROTOBUF_EXPORT
void convert_protobuf(
  const ::kwiver::vital::bounding_box_d& bbox,
  ::kwiver::protobuf::bounding_box&      proto_bbox );

// ---- detected_object
KWIVER_SERIALIZE_PROTOBUF_EXPORT
void convert_protobuf(
  const ::kwiver::protobuf::detected_object& proto_det_object,
  ::kwiver::vital::detected_object&          det_object );

KWIVER_SERIALIZE_PROTOBUF_EXPORT
void convert_protobuf(
  const ::kwiver::vital::detected_object&  det_object,
  ::kwiver::protobuf::detected_object&     proto_det_object );

// ---- detected_object_set
KWIVER_SERIALIZE_PROTOBUF_EXPORT
void convert_protobuf(
  const ::kwiver::protobuf::detected_object_set& proto_dos,
  ::kwiver::vital::detected_object_set&          dos );

KWIVER_SERIALIZE_PROTOBUF_EXPORT
void convert_protobuf(
  const ::kwiver::vital::detected_object_set&  dos,
  ::kwiver::protobuf::detected_object_set&     proto_dos );

// ---- detected_object
KWIVER_SERIALIZE_PROTOBUF_EXPORT
void convert_protobuf(
  const ::kwiver::protobuf::detected_object_type&  proto_dot,
  ::kwiver::vital::detected_object_type&           dot );

KWIVER_SERIALIZE_PROTOBUF_EXPORT
void convert_protobuf(
  const ::kwiver::vital::detected_object_type& dot,
  ::kwiver::protobuf::detected_object_type&    proto_dot );

// ---- geo_polygon
KWIVER_SERIALIZE_PROTOBUF_EXPORT
void convert_protobuf(
  const ::kwiver::protobuf::geo_polygon& proto_poly,
  ::kwiver::vital::geo_polygon&          poly );

KWIVER_SERIALIZE_PROTOBUF_EXPORT
void convert_protobuf(
  const ::kwiver::vital::geo_polygon&  poly,
  ::kwiver::protobuf::geo_polygon&     proto_poly );

// ---- geo_point
KWIVER_SERIALIZE_PROTOBUF_EXPORT
void convert_protobuf(
  const ::kwiver::protobuf::geo_point& proto_point,
  ::kwiver::vital::geo_point&          point );

KWIVER_SERIALIZE_PROTOBUF_EXPORT
void convert_protobuf(
  const ::kwiver::vital::geo_point&  point,
  ::kwiver::protobuf::geo_point&     proto_point );

// ---- polygon
KWIVER_SERIALIZE_PROTOBUF_EXPORT
void convert_protobuf(
  const ::kwiver::protobuf::polygon& proto_poly,
  ::kwiver::vital::polygon&          poly );

KWIVER_SERIALIZE_PROTOBUF_EXPORT
void convert_protobuf(
  const ::kwiver::vital::polygon&  poly,
  ::kwiver::protobuf::polygon&     proto_poly );

// ---- image container
KWIVER_SERIALIZE_PROTOBUF_EXPORT
void convert_protobuf(
  const ::kwiver::protobuf::image&       proto_img,
  ::kwiver::vital::image_container_sptr& img );

KWIVER_SERIALIZE_PROTOBUF_EXPORT
void convert_protobuf(
  const ::kwiver::vital::image_container_sptr img,
  ::kwiver::protobuf::image&                   proto_img  );

// ---- timestamp
KWIVER_SERIALIZE_PROTOBUF_EXPORT
void convert_protobuf(
  const ::kwiver::protobuf::timestamp& proto_tstamp,
  ::kwiver::vital::timestamp&          tstamp );

KWIVER_SERIALIZE_PROTOBUF_EXPORT
void convert_protobuf(
  const ::kwiver::vital::timestamp&  tstamp,
  ::kwiver::protobuf::timestamp&     proto_tstamp );

// ---- metadata vector
KWIVER_SERIALIZE_PROTOBUF_EXPORT
void convert_protobuf(
  const ::kwiver::protobuf::metadata_vector&  proto_mvec,
  ::kwiver::vital::metadata_vector& mvec );

KWIVER_SERIALIZE_PROTOBUF_EXPORT
void convert_protobuf(
  const ::kwiver::vital::metadata_vector& mvec,
  ::kwiver::protobuf::metadata_vector&  proto_mvec );

// ---- Single metadata collection
KWIVER_SERIALIZE_PROTOBUF_EXPORT
void convert_protobuf(
  const ::kwiver::protobuf::metadata&  proto,
  ::kwiver::vital::metadata& metadata );

KWIVER_SERIALIZE_PROTOBUF_EXPORT
void convert_protobuf(
  const ::kwiver::vital::metadata& metadata,
  ::kwiver::protobuf::metadata&  proto );

// ---- string
KWIVER_SERIALIZE_PROTOBUF_EXPORT
void convert_protobuf(
  const ::kwiver::protobuf::string&  proto_string,
  std::string&  str );

KWIVER_SERIALIZE_PROTOBUF_EXPORT
void convert_protobuf(
  const std::string& str,
  ::kwiver::protobuf::string&  proto_string );
// ---- track state
KWIVER_SERIALIZE_PROTOBUF_EXPORT
void convert_protobuf(
  const ::kwiver::protobuf::track_state& proto_trk_state,
  ::kwiver::vital::track_state& trk_state );

KWIVER_SERIALIZE_PROTOBUF_EXPORT
void convert_protobuf(
  const ::kwiver::vital::track_state& trk_state,
  ::kwiver::protobuf::track_state& proto_trk_state );

// ---- object track state
KWIVER_SERIALIZE_PROTOBUF_EXPORT
void convert_protobuf(
  const ::kwiver::protobuf::object_track_state& proto_obj_trk_state,
  ::kwiver::vital::object_track_state& obj_trk_state );

KWIVER_SERIALIZE_PROTOBUF_EXPORT
void convert_protobuf(
  const ::kwiver::vital::object_track_state& obj_trk_state,
  ::kwiver::protobuf::object_track_state& proto_obj_trk_state );

// ---- track
KWIVER_SERIALIZE_PROTOBUF_EXPORT
void convert_protobuf(
  const ::kwiver::protobuf::track& proto_trk,
  ::kwiver::vital::track_sptr& trk_sptr );

KWIVER_SERIALIZE_PROTOBUF_EXPORT
void convert_protobuf(
  const ::kwiver::vital::track_sptr& trk_sptr,
  ::kwiver::protobuf::track& proto_trk );

// ---- track set
KWIVER_SERIALIZE_PROTOBUF_EXPORT
void convert_protobuf(
  const ::kwiver::protobuf::track_set& proto_trk_set,
  ::kwiver::vital::track_set_sptr& trk_set_sptr );

KWIVER_SERIALIZE_PROTOBUF_EXPORT
void convert_protobuf(
  const ::kwiver::vital::track_set_sptr& trk_set_sptr,
  ::kwiver::protobuf::track_set& proto_trk_set );

// ---- object track set
KWIVER_SERIALIZE_PROTOBUF_EXPORT
void convert_protobuf(
  const ::kwiver::protobuf::object_track_set& proto_obj_trk_set,
  ::kwiver::vital::object_track_set_sptr& obj_trk_set_sptr );

KWIVER_SERIALIZE_PROTOBUF_EXPORT
void convert_protobuf(
  const ::kwiver::vital::object_track_set_sptr& obj_trk_set_sptr,
  ::kwiver::protobuf::object_track_set& proto_obj_trk_set );

} // namespace protobuf

} // namespace serialize

} // namespace arrows

}        // end namespace

#endif // ARROWS_SERIALILIZATION_PROTOBUF_CONVERT_PROTOBUF_H
