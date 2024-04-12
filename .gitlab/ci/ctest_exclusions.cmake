set(test_exclusions)

if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "windows")
  list(APPEND test_exclusions
    # See issue #9
    "^qt:image_io.type<struct image_type<unsigned char,1,24>>$"
    "^qt:image_io.type<struct image_type<unsigned char,3,13>>$"
    "^qt:image_io.type<struct image_type<bool,1,1>>$"

    # See issue #10
    "^ffmpeg:ffmpeg_video_output.round_trip$"
    "^ffmpeg:ffmpeg_video_output.round_trip_direct$"
    )
endif ()

# format:
# Add one per line:
# "^name_of_test$"
list(APPEND test_exclusions
  )

string(REPLACE ";" "|" test_exclusions "${test_exclusions}")
if (test_exclusions)
  set(test_exclusions "(${test_exclusions})")
endif ()
