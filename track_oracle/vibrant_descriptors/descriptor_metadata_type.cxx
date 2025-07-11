// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include "descriptor_metadata_type.h"

using std::ostream;
using std::istream;
using std::ios;

namespace kwiver {

namespace track_oracle {

ostream&
operator<<( ostream& os, const descriptor_metadata_type& )
{
  return os;
}

istream&
operator>>( istream& is, descriptor_metadata_type& )
{
  return is;
}

} // ...track_oracle

} // ...kwiver
