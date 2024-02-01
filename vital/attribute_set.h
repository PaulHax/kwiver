// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Alias of types::attribute_set

#ifndef KWIVER_VITAL_ATTRIBUTE_SET_ROOT_H
#define KWIVER_VITAL_ATTRIBUTE_SET_ROOT_H

#include <vital/types/attribute_set.h>

namespace kwiver {
namespace vital {

// When types moved to a separate library, attribute_set needed to move
// to the types library to avoid a circular dependency. This alias avoids
// renaming attribute_set everywhere.
using attribute_set = vital::types::attribute_set;
using attribute_set_exception = vital::types::attribute_set_exception;
using attribute_set_sptr = vital::types::attribute_set_sptr;

} } // end namespace

#endif // KWIVER_VITAL_ATTRIBUTE_SET_ROOT_H
