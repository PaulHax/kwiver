#
# CMake file for the kwiver tool runner
#
if (SKBUILD)
 return()
endif ()

kwiver_add_executable( demo_macro_magic demo_macro_magic.cxx )
target_link_libraries(
  demo_macro_magic
  PRIVATE   vital
)

kwiver_add_executable( demo_python_impl_call demo_python_impl_call.cxx )
target_link_libraries(
  demo_python_impl_call
  PRIVATE   vital
)

configure_file(
  demo_python_impl_call.py
  ${KWIVER_BINARY_DIR}/bin/demo_python_impl_call.py
  COPYONLY
)

###
# built the tool runner
kwiver_add_executable( kwiver
           kwiver_tool_runner.cxx
)

target_link_libraries( kwiver
  PRIVATE        vital
                 vital_vpm
                 vital_logger
                 kwiversys
                 #wiver_tools_applet
                 vital_applets
)


if (KWIVER_ENABLE_PYTHON)
### Add demos as tests
  add_test(
    NAME CXX::demo_python_impl_call
    COMMAND ${KWIVER_BINARY_DIR}/bin/demo_python_impl_call
  )

  add_test(
    NAME Python::demo_python_impl_call
    COMMAND ${Python_EXECUTABLE} ${KWIVER_BINARY_DIR}/bin/demo_python_impl_call.py
  )
endif ()
