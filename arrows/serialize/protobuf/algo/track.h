// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#ifndef ARROWS_SERIALIZATION_PROTO_TRACK_H
#define ARROWS_SERIALIZATION_PROTO_TRACK_H

#include <arrows/serialize/protobuf/kwiver_serialize_protobuf_export.h>
#include <vital/algo/data_serializer.h>

#include <vital/algo/algorithm.txx>

namespace kwiver {

namespace arrows {

namespace serialize {

namespace protobuf {

class KWIVER_SERIALIZE_PROTOBUF_EXPORT track
  : public vital::algo::data_serializer
{
public:
  PLUGGABLE_IMPL(
    track,
    "Serializes a track using protobuf notation. "
    "This implementation only handles a single data item."
  );

  virtual ~track();

  std::shared_ptr< std::string > serialize(
    const vital::any& element ) override;
  vital::any deserialize( const std::string& message ) override;

protected:
  void initialize() override;
};

} // namespace protobuf

} // namespace serialize

} // namespace arrows

}             // end namespace kwiver

#endif // ARROWS_SERIALIZATION_TRACK
