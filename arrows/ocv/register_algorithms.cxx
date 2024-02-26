// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief OpenCV algorithm registration implementation

#include <arrows/ocv/kwiver_algo_ocv_plugin_export.h>
#include <vital/plugin_management/plugin_manager.h>

#include <opencv2/opencv_modules.hpp>
#ifdef HAVE_OPENCV_NONFREE
#include <opencv2/nonfree/nonfree.hpp>
#endif

#include <arrows/ocv/analyze_tracks.h>
#include <arrows/ocv/detect_features_AGAST.h>
#include <arrows/ocv/detect_features_FAST.h>
#include <arrows/ocv/detect_features_GFTT.h>
#include <arrows/ocv/detect_features_MSD.h>
#include <arrows/ocv/detect_features_MSER.h>
#include <arrows/ocv/detect_features_simple_blob.h>
#include <arrows/ocv/detect_features_STAR.h>
#include <arrows/ocv/draw_detected_object_set.h>
#include <arrows/ocv/draw_tracks.h>
#include <arrows/ocv/estimate_fundamental_matrix.h>
#include <arrows/ocv/estimate_homography.h>
#include <arrows/ocv/estimate_pnp.h>
// #include <arrows/ocv/resection_camera.h>
#include <arrows/ocv/extract_descriptors_BRIEF.h>
#include <arrows/ocv/extract_descriptors_DAISY.h>
#include <arrows/ocv/extract_descriptors_FREAK.h>
#include <arrows/ocv/extract_descriptors_LATCH.h>
#include <arrows/ocv/extract_descriptors_LUCID.h>
#include <arrows/ocv/feature_detect_extract_BRISK.h>
#include <arrows/ocv/feature_detect_extract_ORB.h>
#include <arrows/ocv/feature_detect_extract_SIFT.h>
#include <arrows/ocv/feature_detect_extract_SURF.h>
#include <arrows/ocv/image_io.h>

#include <arrows/ocv/inpaint.h>
// #include <arrows/ocv/match_features_bruteforce.h>
// #include <arrows/ocv/match_features_flannbased.h>
// #include <arrows/ocv/merge_images.h>
// #include <arrows/ocv/hough_circle_detector.h>
// #include <arrows/ocv/refine_detections_write_to_disk.h>
// #include <arrows/ocv/split_image.h>
// #include <arrows/ocv/track_features_klt.h>
#include <arrows/ocv/detect_motion_3frame_differencing.h>
#include <arrows/ocv/detect_motion_mog2.h>

// #include <arrows/ocv/detect_heat_map.h>

namespace kwiver {

namespace arrows {

namespace ocv {

extern "C"
KWIVER_ALGO_OCV_PLUGIN_EXPORT
void
register_factories( kwiver::vital::plugin_loader& vpm )
{
  using kvpf = ::kwiver::vital::plugin_factory;

#if defined( HAVE_OPENCV_NONFREE )
  cv::initModule_nonfree();
#endif

  auto fact = vpm.add_factory< vital::algo::analyze_tracks,
    analyze_tracks >( "ocv" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_ocv" );
  fact = vpm.add_factory< vital::algo::draw_tracks, draw_tracks >( "ocv" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_ocv" );
  fact = vpm.add_factory< vital::algo::estimate_fundamental_matrix,
    estimate_fundamental_matrix >( "ocv" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_ocv" );
  fact = vpm.add_factory< vital::algo::estimate_homography,
    estimate_homography >( "ocv" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_ocv" );
  fact = vpm.add_factory< vital::algo::image_io, image_io >( "ocv" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_ocv" );
  fact = vpm.add_factory< vital::algo::merge_images,
    inpaint >( "ocv_inpainting" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_ocv" );
  fact = vpm.add_factory< vital::algo::draw_detected_object_set,
    draw_detected_object_set >( "ocv" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_ocv" );

  fact = vpm.add_factory< vital::algo::detect_features,
    detect_features_BRISK >( "ocv_BRISK" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_ocv" );
  fact = vpm.add_factory< vital::algo::detect_features,
    detect_features_FAST >( "ocv_FAST" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_ocv" );
  fact = vpm.add_factory< vital::algo::detect_features,
    detect_features_GFTT >( "ocv_GFTT" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_ocv" );
  fact = vpm.add_factory< vital::algo::detect_features,
    detect_features_MSER >( "ocv_MSER" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_ocv" );
  fact = vpm.add_factory< vital::algo::detect_features,
    detect_features_ORB >( "ocv_ORB" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_ocv" );
  fact = vpm.add_factory< vital::algo::detect_features,
    detect_features_simple_blob >( "ocv_simple_blob" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_ocv" );

  fact = vpm.add_factory< vital::algo::extract_descriptors,
    extract_descriptors_BRISK >( "ocv_BRISK" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_ocv" );
  fact = vpm.add_factory< vital::algo::extract_descriptors,
    extract_descriptors_ORB >( "ocv_ORB" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_ocv" );
//
//  reg.register_algorithm< match_features_bruteforce >();
//  reg.register_algorithm< match_features_flannbased >();
//
//  reg.register_algorithm< hough_circle_detector >();
  fact = vpm.add_factory< vital::algo::detect_motion,
    detect_motion_3frame_differencing >( "ocv_3frame_differencing" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_ocv" );
  fact = vpm.add_factory< vital::algo::detect_motion,
    detect_motion_mog2 >( "ocv_mog2" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_ocv" );
//
//  // Conditional algorithms
//  // Source ``KWIVER_OCV_HAS_*`` symbol definitions can be found in the header
//  //  files of the algorithms referred to.
#ifdef KWIVER_OCV_HAS_AGAST
  fact = vpm.add_factory< vital::algo::detect_features,
    detect_features_AGAST >( "ocv_AGAST" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_ocv" );
#endif

#ifdef KWIVER_OCV_HAS_BRIEF
  fact = vpm.add_factory< vital::algo::extract_descriptors,
    extract_descriptors_BRIEF >( "ocv_BRIEF" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_ocv" );
#endif

#ifdef KWIVER_OCV_HAS_DAISY
  fact = vpm.add_factory< vital::algo::extract_descriptors,
    extract_descriptors_DAISY >( "ocv_DAISY" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_ocv" );
#endif

#ifdef KWIVER_OCV_HAS_FREAK
  fact = vpm.add_factory< vital::algo::extract_descriptors,
    extract_descriptors_FREAK >( "ocv_FREAK" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_ocv" );
    << << << < HEAD
#endif

#ifdef KWIVER_OCV_HAS_LATCH
  fact = vpm.add_factory< vital::algo::extract_descriptors,
    extract_descriptors_LATCH >( "ocv_LATCH" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_ocv" );
#endif

#ifdef KWIVER_OCV_HAS_LUCID
  fact = vpm.add_factory< vital::algo::extract_descriptors,
    extract_descriptors_LUCID >( "ocv_LUCID" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_ocv" );
#endif

#ifdef KWIVER_OCV_HAS_MSD
  fact = vpm.add_factory< vital::algo::detect_features,
    detect_features_MSD >( "ocv_MSD" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_ocv" );
#endif

#ifdef KWIVER_OCV_HAS_SIFT
  fact = vpm.add_factory< vital::algo::detect_features,
    detect_features_SIFT >( "ocv_SIFT" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_ocv" );
  fact = vpm.add_factory< vital::algo::extract_descriptors,
    extract_descriptors_SIFT >( "ocv_SIFT" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_ocv" );
#endif

#ifdef KWIVER_OCV_HAS_STAR
  fact = vpm.add_factory< vital::algo::detect_features,
    detect_features_STAR >( "ocv_STAR" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_ocv" );
#endif

#ifdef KWIVER_OCV_HAS_SURF
  fact = vpm.add_factory< vital::algo::detect_features,
    detect_features_SURF >( "ocv_SURF" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_ocv" );
  fact = vpm.add_factory< vital::algo::extract_descriptors,
    extract_descriptors_SURF >( "ocv_SURF" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_ocv" );
#endif

//  reg.register_algorithm< detect_heat_map >();
//
//  reg.register_algorithm< refine_detections_write_to_disk >();
//  reg.register_algorithm< split_image >();
//  reg.register_algorithm< merge_images >();
//  reg.register_algorithm< track_features_klt >();
  fact = vpm.add_factory< vital::algo::estimate_pnp, estimate_pnp >( "ocv" );
  fact->add_attribute( kvpf::PLUGIN_MODULE_NAME, "arrows_ocv" );
//  reg.register_algorithm< resection_camera >();
//
//  reg.mark_module_as_loaded();
}

} // end namespace ocv

} // end namespace arrows

} // end namespace kwiver
