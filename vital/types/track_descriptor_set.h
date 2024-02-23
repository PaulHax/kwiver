// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#ifndef VITAL_TRACK_DESCRIPTOR_SET_
#define VITAL_TRACK_DESCRIPTOR_SET_

#include <vital/types/vital_types_export.h>
#include <vital/vital_config.h>

#include <vital/types/track_descriptor.h>

#include <memory>
#include <string>
#include <vector>

namespace kwiver {

namespace vital {

typedef std::vector< track_descriptor_sptr > track_descriptor_set;
typedef std::shared_ptr< track_descriptor_set > track_descriptor_set_sptr;

} // namespace vital

}   // end namespace

#endif
