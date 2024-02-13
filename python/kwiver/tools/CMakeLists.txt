configure_file(
  dump_klv.py
  ${KWIVER_BINARY_DIR}/bin/dump_klv.py
  COPYONLY
)

if( KWIVER_ENABLE_PYTHON_TESTS )
  add_subdirectory(tests)
endif()
