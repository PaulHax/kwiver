include(kwiver-test-setup)

if (KWIVER_ENABLE_FFMPEG)
  if (KWIVER_ENABLE_OPENCV)
    # depends on arrows::core::track_features
    add_test(NAME mvg:applets:track_features
      COMMAND kwiver track-features
                --config "${kwiver_test_data_directory}/config_files/track_features_testing.conf"
                "${kwiver_test_data_directory}/videos/aphill_240_1fps_crf32.mp4")
  endif()

  # depends on arrows::vxl
  add_test(NAME mvg:applets:init_cameras_landmarks
    COMMAND kwiver init-cameras-landmarks
              --config "${kwiver_test_data_directory}/config_files/init_cameras_landmarks_testing.conf"
              --video "${kwiver_test_data_directory}/videos/aphill_240_1fps_crf32.mp4"
              --tracks "${kwiver_test_data_directory}/aphill_pipeline_data/tracks_light.txt")
endif()
