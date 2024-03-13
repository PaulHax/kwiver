// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Defaults plugin algorithm registration interface impl

#include <arrows/serialize/protobuf/kwiver_serialize_protobuf_plugin_export.h>
#include <vital/plugin_management/plugin_manager.h>

#include "activity.h"
#include "activity_type.h"
#include "bounding_box.h"
#include "detected_object.h"
#include "detected_object_set.h"
#include "detected_object_type.h"
#include "image.h"
#include "metadata.h"
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

namespace protobuf {

// ----------------------------------------------------------------------------
extern "C"
KWIVER_SERIALIZE_PROTOBUF_PLUGIN_EXPORT
void
register_factories( kwiver::vital::plugin_loader& vpm )
{
  using kvpf = ::kwiver::vital::plugin_factory;

  auto fact = vpm.add_factory< vital::algo::data_serializer,
    kwiver::arrows::serialize::protobuf::activity >( "kwiver::activity" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_serialize" );

  fact = vpm.add_factory< vital::algo::data_serializer,
    kwiver::arrows::serialize::protobuf::track >( "kwiver::track" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_serialize" );

  fact = vpm.add_factory< vital::algo::data_serializer,
    kwiver::arrows::serialize::protobuf::metadata >( "kwiver::metadata" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_serialize" );

  fact = vpm.add_factory< vital::algo::data_serializer,
    kwiver::arrows::serialize::protobuf::image >( "kwiver::image" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_serialize" );

  fact = vpm.add_factory< vital::algo::data_serializer,
    kwiver::arrows::serialize::protobuf::image >( "kwiver:mask" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_serialize" );

  fact = vpm.add_factory< vital::algo::data_serializer,
    kwiver::arrows::serialize::protobuf::string >( "kwiver::string" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_serialize" );

  fact = vpm.add_factory< vital::algo::data_serializer,
    kwiver::arrows::serialize::protobuf::string >( "kwiver:file_name" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_serialize" );

  fact = vpm.add_factory< vital::algo::data_serializer,
    kwiver::arrows::serialize::protobuf::string >( "kwiver:image_name" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_serialize" );

  fact = vpm.add_factory< vital::algo::data_serializer,
    kwiver::arrows::serialize::protobuf::string >( "kwiver:video_name" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_serialize" );

  fact = vpm.add_factory< vital::algo::data_serializer,
    kwiver::arrows::serialize::protobuf::track_state >( "kwiver::track_state" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_serialize" );

  fact = vpm.add_factory< vital::algo::data_serializer,
    kwiver::arrows::serialize::protobuf::track_set >( "kwiver::track_set" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_serialize" );

  fact = vpm.add_factory< vital::algo::data_serializer,
    kwiver::arrows::serialize::protobuf::timestamp >( "kwiver::timestamp" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_serialize" );

  fact = vpm.add_factory< vital::algo::data_serializer,
    kwiver::arrows::serialize::protobuf::object_track_state >(
           "kwiver::object_track_state" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_serialize" );

  fact = vpm.add_factory< vital::algo::data_serializer,
    kwiver::arrows::serialize::protobuf::object_track_set >(
           "kwiver::object_track_set" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_serialize" );

  fact = vpm.add_factory< vital::algo::data_serializer,
    kwiver::arrows::serialize::protobuf::detected_object_type >(
           "kwiver::detected_object_type" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_serialize" );

  fact = vpm.add_factory< vital::algo::data_serializer,
    kwiver::arrows::serialize::protobuf::detected_object_set >(
           "kwiver::detected_object_set" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_serialize" );

  fact = vpm.add_factory< vital::algo::data_serializer,
    kwiver::arrows::serialize::protobuf::detected_object >(
           "kwiver::detected_object" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_serialize" );

  fact = vpm.add_factory< vital::algo::data_serializer,
    kwiver::arrows::serialize::protobuf::bounding_box >(
           "kwiver::bounding_box" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_serialize" );

  fact = vpm.add_factory< vital::algo::data_serializer,
    kwiver::arrows::serialize::protobuf::activity_type >(
           "kwiver::activity_type" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_serialize" );

/*
 *  using namespace kwiver::arrows::serialize::protobuf;
 *
 *  reg.register_algorithm< bounding_box >();
 *  reg.register_algorithm< activity_type >();
 *  reg.register_algorithm< detected_object_type >();
 *  reg.register_algorithm< detected_object >();
 *  reg.register_algorithm< detected_object_set >();
 *  reg.register_algorithm< timestamp >();
 *  reg.register_algorithm< metadata >();
 *  reg.register_algorithm< image >();
 *  reg.register_algorithm< image >( "kwiver:mask" );
 *  reg.register_algorithm< string >();
 *  reg.register_algorithm< track_state >();
 *  reg.register_algorithm< object_track_state >();
 *  reg.register_algorithm< track >();
 *  reg.register_algorithm< track_set >();
 *  reg.register_algorithm< object_track_set >();
 *  reg.register_algorithm< string >( "kwiver:file_name" );
 *  reg.register_algorithm< string >( "kwiver:image_name" );
 *  reg.register_algorithm< string >( "kwiver:video_name" );
 *
 */
}

} // end namespace protobuf

} // end namespace serialize

} // end namespace arrows

} // end namespace kwiver
