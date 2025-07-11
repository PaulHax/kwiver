// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#ifndef ARROWS_SERIALIZATION_PROTO_IMAGE_H
#define ARROWS_SERIALIZATION_PROTO_IMAGE_H

#include <arrows/serialize/protobuf/kwiver_serialize_protobuf_export.h>
#include <vital/algo/data_serializer.h>

namespace kwiver {

namespace arrows {

namespace serialize {

namespace protobuf {

class KWIVER_SERIALIZE_PROTOBUF_EXPORT image
  : public vital::algo::data_serializer
{
public:
  PLUGGABLE_IMPL(
    image,
    "Serializes an image container using protobuf notation."
  );

  virtual ~image();

  std::shared_ptr< std::string > serialize(
    const vital::any& elements ) override;
  vital::any deserialize( const std::string& message ) override;

protected:
  void initialize() override;
};

} // namespace protobuf

} // namespace serialize

} // namespace arrows

}             // end namespace kwiver

#endif // ARROWS_SERIALIZATION_PROTO_IMAGE_H
