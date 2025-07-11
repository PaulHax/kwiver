set(__test_algorithms
  simple_associate_detections_to_tracks
  simple_bundle_adjust
  simple_close_loops
  simple_compute_association_matrix
  simple_compute_depth
  simple_compute_ref_homography
  simple_compute_stereo_depth_map
  simple_compute_track_descriptors
  simple_convert_image
  simple_detect_features
  simple_detect_motion
  simple_detected_object_filter
  simple_detected_object_set_input
  simple_detected_object_set_output
  simple_draw_detected_object_set
  simple_draw_tracks
  simple_estimate_canonical_transform
  simple_estimate_fundamental_matrix
  simple_estimate_essential_matrix
  simple_estimate_homography
  simple_estimate_pnp
  simple_estimate_similarity_transform
  simple_extract_descriptors
  simple_feature_descriptor_io
  simple_filter_features
  simple_filter_tracks
  simple_image_filter
  simple_image_io
  simple_image_object_detector
  simple_initialize_cameras_landmarks
  simple_initialize_object_tracks
  simple_integrate_depth_maps
  simple_interpolate_track
  simple_keyframe_selection
  simple_match_descriptor_sets
  simple_match_features
  simple_merge_images
  simple_optimize_cameras
  simple_read_object_track_set
  simple_read_track_descriptor_set
  simple_refine_detections
  simple_resection_camera
  simple_split_image
  simple_track_features
  simple_train_detector
  simple_transform_2d_io
  simple_triangulate_landmarks
  simple_uuid_factory
  simple_video_input
  simple_uv_unwrap_mesh
  simple_write_object_track_set
  simple_write_track_descriptor_set
  )

set(test_algorithms
    ${__test_algorithms}
   )

foreach (modname IN LISTS test_algorithms)
  kwiver_add_python_module( ${CMAKE_CURRENT_SOURCE_DIR}/${modname}.py
    vital/tests/alg
    ${modname})
endforeach()

kwiver_add_python_module( ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
  vital/tests/alg
  __init__ )
