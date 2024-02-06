// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Default plugin algorithm registration interface implementation.

#include <arrows/serialize/json/kwiver_serialize_json_plugin_export.h>

#include <vital/plugin_management/plugin_manager.h>

#include "activity.h"
#include "activity_type.h"
#include "bounding_box.h"
#include "detected_object.h"
#include "detected_object_set.h"
#include "detected_object_type.h"
#include "image.h"
#include "metadata.h"
#include "metadata_map_io.h"
#include "object_track_set.h"
#include "object_track_state.h"
#include "string.h"
#include "timestamp.h"
#include "track.h"
#include "track_set.h"
#include "track_state.h"

namespace kwiver {

namespace arrows {

namespace serialize {

namespace json {

// ----------------------------------------------------------------------------
extern "C"
KWIVER_SERIALIZE_JSON_PLUGIN_EXPORT
void
register_factories( kwiver::vital::plugin_loader& vpm )
{
  using namespace kwiver::arrows::serialize::json;

  auto fact =
    vpm.add_factory< vital::algo::data_serializer,
      activity >( "kwiver:activity" );
  fact->add_attribute(
    kwiver::vital::plugin_factory::PLUGIN_MODULE_NAME,
    "arrows.serialize.json" );

  fact = vpm.add_factory< vital::algo::data_serializer,
    activity_type >( "kwiver:activity_type" );
  fact->add_attribute(
    kwiver::vital::plugin_factory::PLUGIN_MODULE_NAME,
    "arrows.serialize.json" );

  fact = vpm.add_factory< vital::algo::data_serializer,
    bounding_box >( "kwiver:bounding_box" );
  fact->add_attribute(
    kwiver::vital::plugin_factory::PLUGIN_MODULE_NAME,
    "arrows.serialize.json" );

  fact = vpm.add_factory< vital::algo::data_serializer,
    detected_object >( "kwiver:detected_object" );
  fact->add_attribute(
    kwiver::vital::plugin_factory::PLUGIN_MODULE_NAME,
    "arrows.serialize.json" );

  fact = vpm.add_factory< vital::algo::data_serializer,
    detected_object_set >( "kwiver:detected_object_set" );
  fact->add_attribute(
    kwiver::vital::plugin_factory::PLUGIN_MODULE_NAME,
    "arrows.serialize.json" );

  fact = vpm.add_factory< vital::algo::data_serializer,
    detected_object_type >( "kwiver:detected_object_type" );
  fact->add_attribute(
    kwiver::vital::plugin_factory::PLUGIN_MODULE_NAME,
    "arrows.serialize.json" );

  fact = vpm.add_factory< vital::algo::data_serializer,
    image >( "kwiver:image" );
  fact->add_attribute(
    kwiver::vital::plugin_factory::PLUGIN_MODULE_NAME,
    "arrows.serialize.json" );

  fact = vpm.add_factory< vital::algo::data_serializer,
    image >( "kwiver:mask" );
  fact->add_attribute(
    kwiver::vital::plugin_factory::PLUGIN_MODULE_NAME,
    "arrows.serialize.json" );

  fact = vpm.add_factory< vital::algo::data_serializer,
    metadata >( "kwiver:metadata" );
  fact->add_attribute(
    kwiver::vital::plugin_factory::PLUGIN_MODULE_NAME,
    "arrows.serialize.json" );

  fact = vpm.add_factory< vital::algo::metadata_map_io,
    metadata_map_io >( "json" );
  fact->add_attribute(
    kwiver::vital::plugin_factory::PLUGIN_MODULE_NAME,
    "arrows.serialize.json" );

  fact = vpm.add_factory< vital::algo::data_serializer,
    object_track_set >( "kwiver:object_track_set" );
  fact->add_attribute(
    kwiver::vital::plugin_factory::PLUGIN_MODULE_NAME,
    "arrows.serialize.json" );

  fact = vpm.add_factory< vital::algo::data_serializer,
    object_track_state >( "kwiver:object_track_state" );
  fact->add_attribute(
    kwiver::vital::plugin_factory::PLUGIN_MODULE_NAME,
    "arrows.serialize.json" );

  fact = vpm.add_factory< vital::algo::data_serializer,
    string >( "kwiver:string" );
  fact->add_attribute(
    kwiver::vital::plugin_factory::PLUGIN_MODULE_NAME,
    "arrows.serialize.json" );

  fact = vpm.add_factory< vital::algo::data_serializer,
    timestamp >( "kwiver:timestamp" );
  fact->add_attribute(
    kwiver::vital::plugin_factory::PLUGIN_MODULE_NAME,
    "arrows.serialize.json" );

  fact = vpm.add_factory< vital::algo::data_serializer,
    track >( "kwiver:track" );
  fact->add_attribute(
    kwiver::vital::plugin_factory::PLUGIN_MODULE_NAME,
    "arrows.serialize.json" );

  fact = vpm.add_factory< vital::algo::data_serializer,
    track_set >( "kwiver:track_set" );
  fact->add_attribute(
    kwiver::vital::plugin_factory::PLUGIN_MODULE_NAME,
    "arrows.serialize.json" );

  fact = vpm.add_factory< vital::algo::data_serializer,
    track_state >( "kwiver:track_state" );
  fact->add_attribute(
    kwiver::vital::plugin_factory::PLUGIN_MODULE_NAME,
    "arrows.serialize.json" );

  fact = vpm.add_factory< vital::algo::data_serializer,
    string >( "kwiver:file_name" );
  fact->add_attribute(
    kwiver::vital::plugin_factory::PLUGIN_MODULE_NAME,
    "arrows.serialize.json" );

  fact = vpm.add_factory< vital::algo::data_serializer,
    string >( "kwiver:image_name" );
  fact->add_attribute(
    kwiver::vital::plugin_factory::PLUGIN_MODULE_NAME,
    "arrows.serialize.json" );

  fact = vpm.add_factory< vital::algo::data_serializer,
    string >( "kwiver:video_name" );
  fact->add_attribute(
    kwiver::vital::plugin_factory::PLUGIN_MODULE_NAME,
    "arrows.serialize.json" );
}

} // end namespace json

} // end namespace serialize

} // end namespace arrows

} // end namespace kwiver
