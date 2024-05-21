project( video_examples )

add_executable( example_video_frames_metadata example_video_frames_metadata.cxx )
target_link_libraries( example_video_frames_metadata
  kwiver_algo_ffmpeg
  )
target_compile_definitions( example_video_frames_metadata
  PRIVATE BINARY_ROOT_DIR="${CMAKE_BINARY_DIR}"
  )
