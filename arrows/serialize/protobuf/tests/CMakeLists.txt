project(kwiver_serialize_protbuf_tests)

include(kwiver-test-setup)

set( test_libraries
  vital    vital_vpm vital_algo    vital_util
  kwiver_serialize_protobuf    vital_types_protobuf )

##############################
# PROTOBUF tests
##############################

kwiver_discover_gtests(serialize-protobuf serialize          LIBRARIES ${test_libraries})
kwiver_discover_gtests(serialize-protobuf serialize_metadata LIBRARIES ${test_libraries})
kwiver_discover_gtests(serialize-protobuf convert_protobuf   LIBRARIES ${test_libraries})
kwiver_discover_gtests(serialize-protobuf activity           LIBRARIES ${test_libraries})
kwiver_discover_gtests(serialize-protobuf bounding_box       LIBRARIES ${test_libraries})
kwiver_discover_gtests(serialize-protobuf detected_object    LIBRARIES ${test_libraries})
kwiver_discover_gtests(serialize-protobuf image              LIBRARIES ${test_libraries})
kwiver_discover_gtests(serialize-protobuf metadata           LIBRARIES ${test_libraries})
kwiver_discover_gtests(serialize-protobuf object_track       LIBRARIES ${test_libraries})
kwiver_discover_gtests(serialize-protobuf string             LIBRARIES ${test_libraries})
kwiver_discover_gtests(serialize-protobuf timestamp          LIBRARIES ${test_libraries})
kwiver_discover_gtests(serialize-protobuf track              LIBRARIES ${test_libraries})
