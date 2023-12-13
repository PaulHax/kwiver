// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/**
 * \file algorithm_implementation.cxx
 *
 * \brief python bindings for algorithm
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

// enable headers as we bring them
// UNCRUST-OFF
// uncrustify breaks long includes in two lines
// #include <python/kwiver/vital/algo/trampoline/activity_detector_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/analyze_tracks_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/associate_detections_to_tracks_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/bundle_adjust_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/close_loops_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/compute_association_matrix_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/compute_depth_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/compute_ref_homography_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/compute_stereo_depth_map_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/convert_image_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/detect_features_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/detect_motion_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/detected_object_filter_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/detected_object_set_input_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/detected_object_set_output_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/draw_detected_object_set_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/draw_tracks_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/estimate_canonical_transform_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/estimate_essential_matrix_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/estimate_fundamental_matrix_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/estimate_homography_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/estimate_pnp_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/estimate_similarity_transform_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/extract_descriptors_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/feature_descriptor_io_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/filter_features_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/filter_tracks_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/image_filter_trampoline.txx>
#include <python/kwiver/vital/algo/trampoline/image_io_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/image_object_detector_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/initialize_cameras_landmarks_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/initialize_object_tracks_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/integrate_depth_maps_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/interpolate_track_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/keyframe_selection_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/match_descriptor_sets_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/match_features_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/merge_images_trampoline.txx>
#include <python/kwiver/vital/algo/trampoline/metadata_map_io_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/optimize_cameras_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/read_object_track_set_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/read_track_descriptor_set_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/refine_detections_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/split_image_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/track_features_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/train_detector_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/transform_2d_io_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/triangulate_landmarks_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/uuid_factory_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/uv_unwrap_mesh_trampoline.txx>
#include <python/kwiver/vital/algo/trampoline/video_input_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/write_object_track_set_trampoline.txx>
// #include <python/kwiver/vital/algo/trampoline/write_track_descriptor_set_trampoline.txx>
// UNCRUST-ON

// enable headers as we bring them
// #include <python/kwiver/vital/algo/activity_detector.h>
#include <python/kwiver/vital/algo/algorithm.h>
#include <python/kwiver/vital/algo/image_io.h>
// #include <python/kwiver/vital/algo/image_object_detector.h>
// #include <python/kwiver/vital/algo/initialize_cameras_landmarks.h>
// #include <python/kwiver/vital/algo/initialize_object_tracks.h>
// #include <python/kwiver/vital/algo/integrate_depth_maps.h>
// #include <python/kwiver/vital/algo/interpolate_track.h>
// #include <python/kwiver/vital/algo/keyframe_selection.h>
// #include <python/kwiver/vital/algo/match_descriptor_sets.h>
// #include <python/kwiver/vital/algo/match_features.h>
// #include <python/kwiver/vital/algo/merge_images.h>
#include <python/kwiver/vital/algo/metadata_map_io.h>
// #include <python/kwiver/vital/algo/optimize_cameras.h>
// #include <python/kwiver/vital/algo/read_object_track_set.h>
// #include <python/kwiver/vital/algo/read_track_descriptor_set.h>
// #include <python/kwiver/vital/algo/refine_detections.h>
// #include <python/kwiver/vital/algo/split_image.h>
// #include <python/kwiver/vital/algo/track_features.h>
// #include <python/kwiver/vital/algo/train_detector.h>
#include <python/kwiver/vital/algo/video_input.h>
// #include <python/kwiver/vital/algo/write_object_track_set.h>
// #include <python/kwiver/vital/algo/write_track_descriptor_set.h>
#include <sstream>

namespace kwiver {

namespace vital {

namespace python {

namespace py = pybind11;
using namespace kwiver::vital::python;

PYBIND11_MODULE( algos, m )
{
  algorithm( m );
  // activity_detector( m );
  // analyze_tracks( m );
  // associate_detections_to_tracks( m );
  // bundle_adjust( m );
  // close_loops( m );
  // compute_association_matrix( m );
  // compute_depth( m );
  // compute_ref_homography( m );
  // compute_stereo_depth_map( m );
  // convert_image( m );
  // detected_object_filter( m );
  // detected_object_set_input( m );
  // detected_object_set_output( m );
  // detect_features( m );
  // detect_motion( m );
  // draw_detected_object_set( m );
  // draw_tracks( m );
  // estimate_canonical_transform( m );
  // estimate_essential_matrix( m );
  // estimate_fundamental_matrix( m );
  // estimate_homography( m );
  // estimate_pnp( m );
  // estimate_similarity_transform( m );
  // extract_descriptors( m );
  // feature_descriptor_io( m );
  // filter_features( m );
  // filter_tracks( m );
  // image_filter( m );
  image_io( m );
  // image_object_detector( m );
  // initialize_cameras_landmarks( m );
  // initialize_object_tracks( m );
  // integrate_depth_maps( m );
  // interpolate_track( m );
  // keyframe_selection( m );
  // match_descriptor_sets( m );
  // match_features( m );
  // merge_images( m );
  metadata_map_io( m );
  // optimize_cameras( m );
  // read_object_track_set( m );
  // read_track_descriptor_set( m );
  // refine_detections( m );
  // split_image( m );
  // track_features( m );
  // train_detector( m );
  // transform_2d_io( m );
  // triangulate_landmarks( m );
  // uuid_factory( m );
  // uv_unwrap_mesh( m );
  video_input( m );
  // write_object_track_set( m );
  // write_track_descriptor_set( m );
}

} // namespace python

} // namespace vital

} // namespace kwiver
