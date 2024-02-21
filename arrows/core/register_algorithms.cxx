// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Register core algorithms implementation

#include <arrows/core/kwiver_algo_core_plugin_export.h>

#include <arrows/core/kwiver_algo_core_export.h>
#include <vital/plugin_management/plugin_manager.h>

// interface
#include <vital/algo/associate_detections_to_tracks.h>
#include <vital/algo/convert_image.h>
#include <vital/algo/detect_features.h>
#include <vital/algo/detected_object_set_input.h>
#include <vital/algo/detected_object_set_output.h>
#include <vital/algo/dynamic_configuration.h>
#include <vital/algo/estimate_canonical_transform.h>
#include <vital/algo/feature_descriptor_io.h>
#include <vital/algo/filter_features.h>
#include <vital/algo/filter_tracks.h>
#include <vital/algo/handle_descriptor_request.h>
#include <vital/algo/image_object_detector.h>
#include <vital/algo/interpolate_track.h>
#include <vital/algo/metadata_filter.h>
#include <vital/algo/metadata_map_io.h>
#include <vital/algo/read_object_track_set.h>
#include <vital/algo/read_track_descriptor_set.h>
#include <vital/algo/uv_unwrap_mesh.h>
#include <vital/algo/video_input.h>

// implementation
#include <arrows/core/associate_detections_to_tracks_threshold.h>
#include <arrows/core/convert_image_bypass.h>
#include <arrows/core/create_detection_grid.h>
#include <arrows/core/derive_metadata.h>
#include <arrows/core/detect_features_filtered.h>
#include <arrows/core/detected_object_set_input_csv.h>
#include <arrows/core/detected_object_set_input_kw18.h>
#include <arrows/core/detected_object_set_input_simulator.h>
#include <arrows/core/detected_object_set_output_csv.h>
#include <arrows/core/dynamic_config_none.h>
#include <arrows/core/estimate_canonical_transform.h>
#include <arrows/core/example_detector.h>
#include <arrows/core/feature_descriptor_io.h>
#include <arrows/core/filter_features_magnitude.h>
#include <arrows/core/filter_features_nonmax.h>
#include <arrows/core/filter_features_scale.h>
#include <arrows/core/filter_tracks.h>
#include <arrows/core/handle_descriptor_request_core.h>
#include <arrows/core/interpolate_track_spline.h>
#include <arrows/core/metadata_map_io_csv.h>
#include <arrows/core/read_object_track_set_kw18.h>
#include <arrows/core/read_track_descriptor_set_csv.h>
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

  fact = vpl.add_factory< vital::algo::filter_features,
    filter_features_magnitude >( "magnitude" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_core" );

  fact = vpl.add_factory< vital::algo::filter_features,
    filter_features_nonmax >( "nonmax" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_core" );

  fact = vpl.add_factory< vital::algo::filter_tracks,
    filter_tracks >( "core" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_core" );

  fact = vpl.add_factory< vital::algo::detected_object_set_input,
    detected_object_set_input_kw18 >( "kw18" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_core" );

  fact = vpl.add_factory< vital::algo::detected_object_set_input,
    detected_object_set_input_csv >( "csv" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_core" );

  fact = vpl.add_factory< vital::algo::detected_object_set_input,
    detected_object_set_input_simulator >( "simulator" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_core" );

  fact = vpl.add_factory< vital::algo::detected_object_set_output,
    detected_object_set_output_csv >( "csv" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_core" );

  fact = vpl.add_factory< vital::algo::dynamic_configuration,
    dynamic_config_none >( "none" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_core" );

  fact = vpl.add_factory< vital::algo::read_track_descriptor_set,
    read_track_descriptor_set_csv >( "csv" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_core" );

  fact = vpl.add_factory< vital::algo::read_object_track_set,
    read_object_track_set_kw18 >( "kw18" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_core" );

  fact = vpl.add_factory< vital::algo::associate_detections_to_tracks,
    associate_detections_to_tracks_threshold >( "threshold" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_core" );

  fact = vpl.add_factory< vital::algo::image_object_detector,
    create_detection_grid >( "create_detection_grid" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_core" );

  fact = vpl.add_factory< vital::algo::convert_image,
    convert_image_bypass >( "bypass" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_core" );

  fact = vpl.add_factory< vital::algo::metadata_filter,
    derive_metadata >( "derive_metadata" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_core" );

  fact = vpl.add_factory< vital::algo::detect_features,
    detect_features_filtered >( "filtered" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_core" );

  fact = vpl.add_factory< vital::algo::estimate_canonical_transform,
    estimate_canonical_transform >( "core pca" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_core" );

  fact = vpl.add_factory< vital::algo::image_object_detector,
    example_detector >( "example detector" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_core" );

  fact = vpl.add_factory< vital::algo::feature_descriptor_io,
    feature_descriptor_io >( "core" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_core" );

  fact = vpl.add_factory< vital::algo::handle_descriptor_request,
    handle_descriptor_request_core >( "core" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_core" );

  fact = vpl.add_factory< vital::algo::interpolate_track,
    interpolate_track_spline >( "spline" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_core" );
}

} // end namespace core

} // end namespace arrows

} // end namespace kwiver
