// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief register vtk applets into a plugin

#include <arrows/vtk/applets/kwiver_algo_vtk_applets_export.h>
#include <vital/plugin_management/plugin_loader.h>

#ifdef VTK_ENABLE_COLOR_MESH
#include <arrows/vtk/applets/color_mesh.h>
#endif
#include <arrows/vtk/applets/estimate_depth.h>
#include <arrows/vtk/applets/fuse_depth.h>

namespace kwiver {

namespace arrows {

namespace vtk {

// ----------------------------------------------------------------------------
extern "C"
KWIVER_ALGO_VTK_APPLETS_EXPORT
void
register_factories( kwiver::vital::plugin_loader& vpl )
{
  using namespace kwiver::tools;
  using kvpf = ::kwiver::vital::plugin_factory;

  // make sure the attributes are the same for all applets
  auto set_fact_attributes = [](::kwiver::vital::plugin_factory_handle_t fact){
                               fact->add_attribute(
                                 kvpf::PLUGIN_DESCRIPTION,
                                 "Kwiver arrow vtk applets" )
                                                                                  .
                                 add_attribute(
                                   kvpf::PLUGIN_MODULE_NAME,
                                   "arrows.vtk.applets" )
                                                                                  .
                                 add_attribute(
                                   kvpf::ALGORITHM_CATEGORY,
                                   kvpf::APPLET_CATEGORY );
                             };
  auto fact =
    vpl.add_factory< kwiver_applet,
      estimate_depth >( "estimate-depth" );
  set_fact_attributes( fact );
  fact =
    vpl.add_factory< kwiver_applet, fuse_depth >( "fuse-depth" );
  set_fact_attributes( fact );
#ifdef VTK_ENABLE_COLOR_MESH
  fact =
    vpl.add_factory< kwiver_applet,
      color_mesh >( "color-mesh" );
  set_fact_attributes( fact );
#endif
}

} // end namespace vtk

} // end namespace arrows

} // end namespace kwiver
