project(arrows_test_vxl)

set(CMAKE_FOLDER "Arrows/VXL/Tests")

include(kwiver-test-setup)

set(test_libraries
  kwiver_algo_vxl
  vil_algo)

##############################
# Algorithms VXL tests
##############################
kwiver_discover_gtests(vxl aligned_edge_detection         LIBRARIES ${test_libraries}  ARGUMENTS "${kwiver_test_data_directory}")
kwiver_discover_gtests(vxl average_frames                 LIBRARIES ${test_libraries}  ARGUMENTS "${kwiver_test_data_directory}")
kwiver_discover_gtests(vxl bounding_box                   LIBRARIES ${test_libraries})
kwiver_discover_gtests(vxl camera                         LIBRARIES ${test_libraries})
kwiver_discover_gtests(vxl close_loops_homography_guided  LIBRARIES ${test_libraries}  ARGUMENTS "${kwiver_test_data_directory}")
kwiver_discover_gtests(vxl color_commonality_filter       LIBRARIES ${test_libraries}  ARGUMENTS "${kwiver_test_data_directory}")
kwiver_discover_gtests(vxl convert_image                  LIBRARIES ${test_libraries}  ARGUMENTS "${kwiver_test_data_directory}")
kwiver_discover_gtests(vxl estimate_canonical_transform   LIBRARIES ${test_libraries})
kwiver_discover_gtests(vxl estimate_homography            LIBRARIES ${test_libraries})
kwiver_discover_gtests(vxl estimate_similarity            LIBRARIES ${test_libraries})
kwiver_discover_gtests(vxl hashed_image_classifier_filter LIBRARIES ${test_libraries}  ARGUMENTS "${kwiver_test_data_directory}")
kwiver_discover_gtests(vxl high_pass_filter               LIBRARIES ${test_libraries}  ARGUMENTS "${kwiver_test_data_directory}")
kwiver_discover_gtests(vxl image                          LIBRARIES ${test_libraries})
kwiver_discover_gtests(vxl image_io                       LIBRARIES ${test_libraries}  ARGUMENTS "${kwiver_test_data_directory}")
kwiver_discover_gtests(vxl kd_tree                        LIBRARIES ${test_libraries})
kwiver_discover_gtests(vxl match_features_constrained     LIBRARIES ${test_libraries})
kwiver_discover_gtests(vxl morphology                     LIBRARIES ${test_libraries}  ARGUMENTS "${kwiver_test_data_directory}")
kwiver_discover_gtests(vxl pixel_feature_extractor        LIBRARIES ${test_libraries}  ARGUMENTS "${kwiver_test_data_directory}")
kwiver_discover_gtests(vxl polygon                        LIBRARIES ${test_libraries})
kwiver_discover_gtests(vxl split_image                    LIBRARIES ${test_libraries})

if( fletch_ENABLE_FFmpeg AND _FFmpeg_version VERSION_LESS 4 )
  kwiver_discover_gtests(vxl vidl_ffmpeg_video_input        LIBRARIES ${test_libraries}  ARGUMENTS "${kwiver_test_data_directory}")
endif()

if( KWIVER_ENABLE_MVG )
  # Additional tests that depend on the MVG arrow
  list(APPEND test_libraries kwiver_algo_mvg)

  kwiver_discover_gtests(vxl bundle_adjust               LIBRARIES ${test_libraries})
  kwiver_discover_gtests(vxl estimate_essential_matrix   LIBRARIES ${test_libraries})
  kwiver_discover_gtests(vxl estimate_fundamental_matrix LIBRARIES ${test_libraries})
  kwiver_discover_gtests(vxl optimize_cameras            LIBRARIES ${test_libraries})
  kwiver_discover_gtests(vxl triangulate_landmarks       LIBRARIES ${test_libraries})

  # This test is really for an algorithm in MVG, but it's here
  # because the test depends on availability of VXL sub-algorithms
  # kwiver_discover_gtests(vxl initialize_cameras_landmarks LIBRARIES ${test_libraries})
endif()
