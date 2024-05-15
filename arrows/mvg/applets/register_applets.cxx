// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/**
 * \file
 * \brief register mvg applets into a plugin
 */

#include <arrows/mvg/applets/kwiver_algo_mvg_applets_export.h>
#include <vital/plugin_management/plugin_loader.h>

#include <arrows/mvg/applets/bundle_adjust_tool.h>
#include <arrows/mvg/applets/init_cameras_landmarks.h>
#include <arrows/mvg/applets/track_features.h>

namespace kwiver {

namespace arrows {

namespace mvg {

// ----------------------------------------------------------------------------
extern "C"
KWIVER_ALGO_MVG_APPLETS_EXPORT
void
register_factories( kwiver::vital::plugin_loader& vpl )
{
  using namespace kwiver::tools;
  using kvpf = ::kwiver::vital::plugin_factory;

  // make sure the attributes are the same for all applets
  auto set_fact_attributes = [](::kwiver::vital::plugin_factory_handle_t fact){
                               fact->add_attribute(
                                 kvpf::PLUGIN_DESCRIPTION,
                                 "Kwiver arrow mvg applets" )
                                                                                  .
                                 add_attribute(
                                   kvpf::PLUGIN_MODULE_NAME,
                                   "arrows.mvg.applets" )
                                                                                  .
                                 add_attribute(
                                   kvpf::ALGORITHM_CATEGORY,
                                   kvpf::APPLET_CATEGORY );
                             };

  auto fact =
    vpl.add_factory< kwiver_applet,
      bundle_adjust_tool >( "bundle-adjust-tool" );
  set_fact_attributes( fact );
  fact =
    vpl.add_factory< kwiver_applet,
      init_cameras_landmarks >( "init-cameras-landmarks" );
  set_fact_attributes( fact );
  fact =
    vpl.add_factory< kwiver_applet, track_features >( "track-features" );
  set_fact_attributes( fact );
}

} // end namespace mvg

} // end namespace arrows

} // end namespace kwiver
