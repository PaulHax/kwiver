// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Register VXL algorithms implementation

#include <arrows/vxl/kwiver_algo_vxl_plugin_export.h>
#include <vital/plugin_management/plugin_manager.h>

// interface
#include <vital/algo/estimate_essential_matrix.h>
#include <vital/algo/estimate_similarity_transform.h>
#include <vital/algo/image_filter.h>
#include <vital/algo/image_io.h>
#include <vital/algo/nearest_neighbors.h>
#include <vital/algo/optimize_cameras.h>

// implementation
#include <arrows/vxl/aligned_edge_detection.h>
#include <arrows/vxl/average_frames.h>
#include <arrows/vxl/bundle_adjust.h>
#include <arrows/vxl/close_loops_homography_guided.h>
#include <arrows/vxl/convert_image.h>
#include <arrows/vxl/estimate_canonical_transform.h>
#include <arrows/vxl/estimate_essential_matrix.h>
#include <arrows/vxl/estimate_similarity_transform.h>
#include <arrows/vxl/image_io.h>
#include <arrows/vxl/kd_tree.h>
#include <arrows/vxl/optimize_cameras.h>

// #include <arrows/vxl/bundle_adjust.h>
// #include <arrows/vxl/close_loops_homography_guided.h>
// #include <arrows/vxl/color_commonality_filter.h>
// #include <arrows/vxl/estimate_fundamental_matrix.h>
// #include <arrows/vxl/estimate_homography.h>
// #include <arrows/vxl/hashed_image_classifier_filter.h>
// #include <arrows/vxl/high_pass_filter.h>
// #include <arrows/vxl/match_features_constrained.h>
// #include <arrows/vxl/morphology.h>
// #include <arrows/vxl/pixel_feature_extractor.h>
// #include <arrows/vxl/split_image.h>
// #include <arrows/vxl/threshold.h>
// #include <arrows/vxl/triangulate_landmarks.h>

#ifdef VXL_ENABLE_FFMPEG
#include <arrows/vxl/vidl_ffmpeg_video_input.h>
#endif

namespace kwiver {

namespace arrows {

namespace vxl {

extern "C"
KWIVER_ALGO_VXL_PLUGIN_EXPORT
void
register_factories( kwiver::vital::plugin_loader& vpl )
{
  using kvpf = ::kwiver::vital::plugin_factory;

  auto fact = vpl.add_factory< vital::algo::image_filter,
    aligned_edge_detection >( "vxl_aligned_edge_detection" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows.vxl" );

  fact = vpl.add_factory< vital::algo::image_filter,
    average_frames >( "vxl_average_frames" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows.vxl" );

  fact = vpl.add_factory< vital::algo::bundle_adjust,
    bundle_adjust >( "vxl_bundle_adjust" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows.vxl" );

  fact = vpl.add_factory< vital::algo::close_loops,
    close_loops_homography_guided >( "vxl_close_loops_homography_guided" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows.vxl" );

  fact = vpl.add_factory< vital::algo::image_filter,
    convert_image >( "vxl_convert_image" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows.vxl" );

  fact = vpl.add_factory< vital::algo::estimate_canonical_transform,
    estimate_canonical_transform >( "vxl_estimate_canonical_transform" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows.vxl" );

  fact = vpl.add_factory< vital::algo::estimate_essential_matrix,
    estimate_essential_matrix >( "vxl" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows.vxl" );

  fact = vpl.add_factory< vital::algo::estimate_similarity_transform,
    estimate_similarity_transform >( "vxl" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows.vxl" );

  fact = vpl.add_factory< vital::algo::image_io,
    image_io >( "vxl" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows.vxl" );

  fact = vpl.add_factory< vital::algo::nearest_neighbors,
    kd_tree >( "vxl_kd_tree" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows.vxl" );

  fact = vpl.add_factory< vital::algo::optimize_cameras,
    optimize_cameras >( "vxl" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows.vxl" );

#ifdef VXL_ENABLE_FFMPEG
  // reg.register_algorithm< vidl_ffmpeg_video_input >();
#endif
}

} // end namespace vxl

} // end namespace arrows

} // end namespace kwiver
