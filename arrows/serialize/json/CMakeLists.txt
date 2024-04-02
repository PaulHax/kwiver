# Build / Install plugin for serialization

if(NOT KWIVER_ENABLE_ZLIB)
  message(FATAL_ERROR "JSON serialization requires zlib (KWIVER_ENABLE_ZLIB)")
endif()
include_directories( ${ZLIB_INCLUDE_DIRS} )

set( headers_public
  algo/activity.h
  algo/activity_type.h
  algo/bounding_box.h
  algo/detected_object.h
  algo/detected_object_set.h
  algo/detected_object_type.h
  algo/image.h
  algo/metadata.h
  algo/metadata_map_io.h
  algo/object_track_set.h
  algo/object_track_state.h
  algo/string.h
  algo/timestamp.h
  algo/track.h
  algo/track_set.h
  algo/track_state.h
  )

set( private_headers
  load_save.h
  load_save_point.h
  load_save_track_state.h
  load_save_track_set.h
  track_item.h
  )

set( sources
  algo/activity.cxx
  algo/activity_type.cxx
  algo/bounding_box.cxx
  algo/detected_object.cxx
  algo/detected_object_set.cxx
  algo/detected_object_type.cxx
  algo/image.cxx
  algo/metadata.cxx
  algo/metadata_map_io.cxx
  algo/object_track_set.cxx
  algo/object_track_state.cxx
  algo/string.cxx
  algo/timestamp.cxx
  algo/track.cxx
  algo/track_set.cxx
  algo/track_state.cxx

  load_save.cxx
  load_save_point.cxx
  load_save_metadata.cxx
  )

kwiver_install_headers(
  SUBDIR     arrows/serialize/json
  ${headers_public}
  )

kwiver_install_headers(
  ${CMAKE_CURRENT_BINARY_DIR}/kwiver_serialize_json_export.h
  NOPATH   SUBDIR     arrows/serialize/json
  )

kwiver_add_library( kwiver_serialize_json
  ${headers_public}
  ${sources}
  )

target_link_libraries( kwiver_serialize_json
  PUBLIC               vital_algo
  PRIVATE              ${ZLIB_LIBRARIES}
  )

algorithms_create_plugin( kwiver_serialize_json
  register_algorithms.cxx
  )

if (KWIVER_ENABLE_TESTS)
  add_subdirectory(tests)
endif()

if (KWIVER_ENABLE_KLV)
  add_subdirectory(klv)
endif()
