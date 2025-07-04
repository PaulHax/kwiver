###
# Build file for abstract algorithms
#

#
set( public_headers
  activity_detector.h
  algorithm.h
  algorithm.txx
  analyze_tracks.h
  associate_detections_to_tracks.h
  buffered_metadata_filter.h
  bundle_adjust.h
  close_loops.h
  compute_association_matrix.h
  compute_depth.h
  compute_ref_homography.h
  compute_stereo_depth_map.h
  compute_track_descriptors.h
  convert_image.h
  data_serializer.h
  detect_features.h
  detect_motion.h
  detected_object_filter.h
  detected_object_set_input.h
  detected_object_set_output.h
  draw_detected_object_set.h
  draw_tracks.h
  dynamic_configuration.h
  estimate_canonical_transform.h
  estimate_essential_matrix.h
  estimate_fundamental_matrix.h
  estimate_homography.h
  estimate_pnp.h
  estimate_similarity_transform.h
  extract_descriptors.h
  feature_descriptor_io.h
  filter_features.h
  filter_tracks.h
  handle_descriptor_request.h
  image_filter.h
  image_io.h
  image_object_detector.h
  initialize_cameras_landmarks.h
  initialize_object_tracks.h
  integrate_depth_maps.h
  interpolate_track.h
  nearest_neighbors.h
  keyframe_selection.h
  metadata_filter.h
  metadata_map_io.h
  match_descriptor_sets.h
  match_features.h
  merge_images.h
  optimize_cameras.h
  pointcloud_io.h
  read_object_track_set.h
  read_track_descriptor_set.h
  refine_detections.h
  resection_camera.h
  split_image.h
  track_features.h
  train_detector.h
  transform_2d_io.h
  triangulate_landmarks.h
  uuid_factory.h
  uv_unwrap_mesh.h
  video_input.h
  video_output.h
  warp_image.h
  write_object_track_set.h
  write_track_descriptor_set.h
  )

set( sources
  activity_detector.cxx
  algorithm.cxx
  analyze_tracks.cxx
  associate_detections_to_tracks.cxx
  buffered_metadata_filter.cxx
  bundle_adjust.cxx
  close_loops.cxx
  compute_association_matrix.cxx
  compute_depth.cxx
  compute_ref_homography.cxx
  compute_stereo_depth_map.cxx
  compute_track_descriptors.cxx
  convert_image.cxx
  data_serializer.cxx
  detect_features.cxx
  detect_motion.cxx
  detected_object_filter.cxx
  detected_object_set_input.cxx
  detected_object_set_output.cxx
  draw_detected_object_set.cxx
  draw_tracks.cxx
  dynamic_configuration.cxx
  estimate_canonical_transform.cxx
  estimate_essential_matrix.cxx
  estimate_fundamental_matrix.cxx
  estimate_homography.cxx
  estimate_pnp.cxx
  estimate_similarity_transform.cxx
  extract_descriptors.cxx
  feature_descriptor_io.cxx
  filter_features.cxx
  filter_tracks.cxx
  handle_descriptor_request.cxx
  image_filter.cxx
  image_io.cxx
  image_object_detector.cxx
  initialize_object_tracks.cxx
  initialize_cameras_landmarks.cxx
  integrate_depth_maps.cxx
  interpolate_track.cxx
  nearest_neighbors.cxx
  keyframe_selection.cxx
  metadata_filter.cxx
  metadata_map_io.cxx
  match_descriptor_sets.cxx
  match_features.cxx
  merge_images.cxx
  optimize_cameras.cxx
  pointcloud_io.cxx
  read_object_track_set.cxx
  read_track_descriptor_set.cxx
  refine_detections.cxx
  resection_camera.cxx
  split_image.cxx
  track_features.cxx
  train_detector.cxx
  transform_2d_io.cxx
  triangulate_landmarks.cxx
  uuid_factory.cxx
  uv_unwrap_mesh.cxx
  video_input.cxx
  video_output.cxx
  warp_image.cxx
  write_object_track_set.cxx
  write_track_descriptor_set.cxx
)

###
## Install header files
kwiver_install_headers(
  ${public_headers}
  SUBDIR   vital/algo
  )

# install export header
kwiver_install_headers(
  ${CMAKE_CURRENT_BINARY_DIR}/vital_algo_export.h
  SUBDIR   vital/algo
  NOPATH
  )

kwiver_add_library( vital_algo SHARED
  ${public_headers}
  ${sources}
  ${CMAKE_CURRENT_BINARY_DIR}/vital_algo_export.h
  )

target_link_libraries( vital_algo
  PRIVATE         kwiversys
                  vital_util
  PUBLIC          vital
                  vital_config
                  vital_logger
                  vital_types
                  vital_vpm
  )

# Needs to be updated for new API
#if (KWIVER_ENABLE_TOOLS)
####
##     plugins for plugin explorer
#kwiver_add_plugin( algo_explorer_plugin
#  SUBDIR   ${kwiver_plugin_plugin_explorer_subdir}
#  SOURCES  algo_explorer_plugin.cxx
#  PRIVATE  vital
#           vital_vpm
#           vital_config
#           vital_algo
#           explorer_plugin )
#endif()
