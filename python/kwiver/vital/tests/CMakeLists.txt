#
# Python support
#

add_subdirectory(cpp_helpers)

# Define modules with tests in them
set(__testable_modnames
  test_activity
  test_algorithms_common
  test_algorithm_factory
  test_activity_type
  test_bounding_box
  test_camera
  test_camera_intrinsics
  test_category_hierarchy
  test_config
  test_camera_map
  test_camera_perspective
  test_camera_perspective_map
  test_camera_rpc
  test_color
  test_covariance
  test_database_query
  test_detected_object_set
  test_descriptor
  test_descriptor_request
  test_descriptor_set
  test_detected_object_type
  test_detected_object
  test_detected_object_set_input
  test_essential_matrix
  test_feature
  test_feature_set
  test_feature_track_set
  test_fundamental_matrix
  test_geo_covariance
  test_geo_MGRS
  test_geo_point
  test_geo_polygon
  test_geodesy
  test_feature_descriptor_io
  test_homography
  test_homography_f2f
  test_homography_f2w
  test_image
  test_image_container
  test_iqr_feedback
  test_image_io
  test_landmark
  test_landmark_map
  test_local_cartesian
  test_local_geo_cs
  test_match_set
  test_mesh
  test_metadata
  test_metadata_map
  test_metadata_tags
  test_metadata_traits
  test_modules
  test_object_track_state
  test_object_track_set
  test_query_result
  test_rotation
  test_point
  test_polygon
  test_sfm_constraints
  test_similarity
  test_timestamp
  test_track
  test_track_descriptor
  test_track_interval
  test_track_set
  test_track_state
  test_transform_2d
  test_transform_2d_io
  test_uid
  )

if (KWIVER_ENABLE_FFMPEG AND
    KWIVER_ENABLE_OPENCV AND
    KWIVER_ENABLE_KLV AND
    KWIVER_ENABLE_SERIALIZE_JSON)
  list(APPEND __testable_modnames burnout_smoke_test)
endif()

# All python filenames_we in the vital.tests module
set(__all_modnames
  __init__
  py_helpers
  ${__testable_modnames}
  )

# Add files to the vital.tests python module
foreach (modname IN LISTS __all_modnames)
  kwiver_add_python_module( ${CMAKE_CURRENT_SOURCE_DIR}/${modname}.py
    vital/tests
    ${modname})
endforeach()

# Add python vital.tests to CTest if python test runner is available
if (KWIVER_ENABLE_PYTHON_TESTS)
  if (WIN32)
    set(WIN_TEST_CONFIG_TYPE "$<CONFIG>")
  endif()

  include(kwiver-setup-tests-python)
  foreach (test IN LISTS __testable_modnames)
    kwiver_add_pytest( "${CMAKE_CURRENT_SOURCE_DIR}/${test}" python)
  endforeach()

  set(_test_data
        "data/cube.ply")
  foreach (file_name IN LISTS _test_data)
    kwiver_python_add_test_data(${file_name} ${kwiver_test_working_path})
  endforeach()
endif()

add_subdirectory(alg)
