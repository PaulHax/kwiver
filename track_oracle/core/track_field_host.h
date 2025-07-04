// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#ifndef INCL_TRACK_FIELD_HOST_H
#define INCL_TRACK_FIELD_HOST_H

#include <track_oracle/core/track_oracle_export.h>
#include <vital/vital_config.h>

#include <track_oracle/core/track_oracle_core.h>

namespace kwiver {

namespace track_oracle {

// this class exists ONLY to supply a common row
// across a row view when a track_field calls its op().

class TRACK_ORACLE_EXPORT track_field_host
{
private:
  mutable oracle_entry_handle_type cursor;

public:
  track_field_host();

  virtual ~track_field_host();

  oracle_entry_handle_type get_cursor() const;

  void set_cursor( oracle_entry_handle_type h ) const;
};

} // ...track_oracle

} // ...kwiver

#endif
