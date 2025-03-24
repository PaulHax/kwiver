// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <arrows/geocalc/geo_conv.h>

#include <arrows/geocalc/kwiver_algo_geocalc_plugin_export.h>
#include <vital/plugin_management/plugin_manager.h>
#include <vital/types/geodesy.h>

namespace kwiver {

namespace arrows {

namespace geocalc {

// ----------------------------------------------------------------------------
extern "C"
KWIVER_ALGO_GEOCALC_PLUGIN_EXPORT
void
register_factories( vital::plugin_loader& vpm )
{
  static auto const module_name = std::string( "arrows.geocalc" );
  if( vpm.is_module_loaded( module_name ) )
  {
    return;
  }

  vpm.mark_module_as_loaded( module_name );
}

}   // end namespace proj

}   // end namespace arrows

}   // end namespace kwiver
