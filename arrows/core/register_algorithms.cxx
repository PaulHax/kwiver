// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Register core algorithms implementation

#include <arrows/core/kwiver_algo_core_plugin_export.h>

#include <arrows/core/kwiver_algo_core_export.h>
#include <vital/plugin_management/plugin_manager.h>

// interface
#include <vital/algo/filter_features.h>
#include <vital/algo/metadata_map_io.h>
#include <vital/algo/uv_unwrap_mesh.h>
#include <vital/algo/video_input.h>

// implementation
#include <arrows/core/filter_features_scale.h>
#include <arrows/core/metadata_map_io_csv.h>
#include <arrows/core/uv_unwrap_mesh.h>
#include <arrows/core/video_input_filter.h>
#include <arrows/core/video_input_image_list.h>
#include <arrows/core/video_input_pos.h>
#include <arrows/core/video_input_split.h>

namespace kwiver {

namespace arrows {

namespace core {

// ----------------------------------------------------------------------------
extern "C"
KWIVER_ALGO_CORE_EXPORT
void
register_factories( kwiver::vital::plugin_loader& vpl )
{
  using kvpf = ::kwiver::vital::plugin_factory;

  auto fact =
    vpl.add_factory< vital::algo::video_input, video_input_filter >( "filter" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_core" );

  fact = vpl.add_factory< vital::algo::uv_unwrap_mesh,
    uv_unwrap_mesh >( "core" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_core" );

  fact = vpl.add_factory< vital::algo::video_input,
    video_input_split >( "split" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_core" );

  fact = vpl.add_factory< vital::algo::video_input,
    video_input_image_list >( "image_list" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_core" );

  fact = vpl.add_factory< vital::algo::video_input, video_input_pos >( "pos" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_core" );

  fact = vpl.add_factory< vital::algo::metadata_map_io,
    metadata_map_io_csv >( "csv" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_core" );

  fact = vpl.add_factory< vital::algo::filter_features,
    filter_features_scale >( "scale" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_core" );
}

} // end namespace core

} // end namespace arrows

} // end namespace kwiver
