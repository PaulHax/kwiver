project(kwiver_serialize_json_tests)

set(CMAKE_FOLDER "Arrows/Serialize/Tests/KLV")

include(kwiver-test-setup)

set( test_libraries vital vital_vpm vital_algo kwiver_serialize_json_klv kwiver_algo_klv )

##############################
# JSON KLV tests
##############################

kwiver_discover_gtests(serialize-json load_save_klv   LIBRARIES ${test_libraries} ARGUMENTS "${kwiver_test_data_directory}")
kwiver_discover_gtests(serialize-json metadata_map_io LIBRARIES ${test_libraries} )
