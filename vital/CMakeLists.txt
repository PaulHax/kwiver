#
# Top level build for VITAL
#
project( vital )
set(CMAKE_FOLDER "Vital")

###
# Vital requirements
# TODO: Eigen (find_package_once? kwiver_find_package?
#       - to allow the creation of standard "depends" like current, but avoid
#         monolithic include at the project root.)

# ==============================================================================
# KWSys
#---------------------------------------------------------------------
# Create the kwsys library for vital.
set(KWSYS_NAMESPACE                kwiversys)
set(KWSYS_USE_DynamicLoader        1)
set(KWSYS_USE_SystemTools          1)
set(KWSYS_USE_System               1)
set(KWSYS_USE_RegularExpression    1)
set(KWSYS_USE_SystemInformation    1)
set(KWSYS_USE_Directory            1)
set(KWSYS_USE_CommandLineArguments 1)
set(KWSYS_USE_MD5 1)

set(KWSYS_INSTALL_EXPORT_NAME  ${kwiver_export_name})
set(KWSYS_INSTALL_INCLUDE_DIR  "include")
set(KWSYS_INSTALL_LIB_DIR      lib)
set(KWSYS_INCLUDE_DIR          ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "")
set(KWSYS_INSTALL_BIN_DIR      bin)

# Save our custom CXX flags and reset so we pass no special flags to kwiversys.
# Our flags are more strict than expected and causes many warnings and errors.
set( save_flags  ${CMAKE_CXX_FLAGS} )
set( CMAKE_CXX_FLAGS )

include_directories( SYSTEM ${KWSYS_INCLUDE_DIR}/kwiversys )
add_subdirectory(kwiversys)
_kwiver_export( ${KWSYS_NAMESPACE} )
get_target_property(kwsys_target_type "${KWSYS_NAMESPACE}" TYPE)
if (kwsys_target_type STREQUAL "STATIC_LIBRARY")
  _kwiver_compile_pic("${KWSYS_NAMESPACE}")
endif()
set_property(GLOBAL APPEND PROPERTY kwiver_libraries kwiversys)

# force install to standard library directory
# Note that some of the following duplicates what is in kwiver_add_library()
set_target_properties( ${KWSYS_NAMESPACE}
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
  INTERFACE_INCLUDE_DIRECTORIES "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/vital/kwiversys>$<INSTALL_INTERFACE:include>"
  )

  if (APPLE)
  set_target_properties( ${KWSYS_NAMESPACE}
    PROPERTIES
    MACOSX_RPATH          TRUE)
else()
  set_target_properties( ${KWSYS_NAMESPACE}
    PROPERTIES
        VERSION          ${KWIVER_VERSION}
        SOVERSION        ${KWIVER_VERSION}
        )
endif()

# restore old CXX flags for our code.
set( CMAKE_CXX_FLAGS ${save_flags} )

# ==============================================================================
# Other VITAL-specific setup and flags

# ==============================================================================
# Vital Library

# ==================================================================
###
# These headers are installed
#
set( vital_public_headers
  vital_types.h

  algorithm_capabilities.h
  any.h
  attribute_set.h
  bitflags.h
  context.h
  exceptions.h
  iterator.h
  math_constants.h
  noncopyable.h
  set.h
  signal.h

  io/camera_from_metadata.h
  io/camera_io.h
  io/camera_map_io.h
  io/eigen_io.h
  io/landmark_map_io.h
  io/mesh_io.h
  io/metadata_io.h
  io/track_set_io.h
)

# ----------------------
set( vital_sources
  algorithm_capabilities.cxx

  io/camera_from_metadata.cxx
  io/camera_io.cxx
  io/camera_map_io.cxx
  io/landmark_map_io.cxx
  io/mesh_io.cxx
  io/metadata_io.cxx
  io/track_set_io.cxx
)

kwiver_install_headers(
  ${vital_public_headers}
  SUBDIR   vital
  )

# install export header
kwiver_install_headers(
  ${CMAKE_CURRENT_BINARY_DIR}/vital_export.h
  ${CMAKE_CURRENT_BINARY_DIR}/vital_config.h
  ${CMAKE_CURRENT_BINARY_DIR}/version.h
  SUBDIR   vital
  NOPATH
  )

kwiver_add_library( vital
  ${vital_public_headers}
  ${vital_sources}
  ${CMAKE_CURRENT_BINARY_DIR}/vital_export.h
  ${CMAKE_CURRENT_BINARY_DIR}/vital_config.h
  )

# Make sure the configuration targets flush before building this library.
add_dependencies (vital kwiver_configure)

target_link_libraries( vital
  PRIVATE         kwiversys
  PUBLIC          vital_config
                  vital_exceptions
                  vital_logger
                  vital_util
                  vital_types
                  vital_vpm
                  vital_applets
                  Eigen3::Eigen
  )

###
#
# This option adds another directory to the path by appending the configuration type to
# existing directories.
set(default OFF)

if (CMAKE_CONFIGURATION_TYPES)
  set(default ON)
endif ()

option(KWIVER_USE_CONFIGURATION_SUBDIRECTORY
  "Look in the configuration's subdirectory for each module path (e.g. debug, release, ...)" ${default})

###
# configure our compiler options
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/vital_config.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/vital_config.h" )

###
# Subdirectory entries
# -- Creates library "vital_exceptions"
add_subdirectory( exceptions )
add_subdirectory( algo )
# -- Creates library "vital_util"
add_subdirectory( util )
add_subdirectory( range )
# -- Creates library "vital_config"
# Requires: vital_logger, vital_util (vital_types)
add_subdirectory( config )
# -- Creates library "vital_logger"
# Requires: vital_util
add_subdirectory( logger )
# -- Creates library "vital_types"
# Requires: vital_config
add_subdirectory( types )
# -- Creates library "vital_vpm"
# Requires: vital_exceptions, vital_logger, vital_util
# Uses common headers: vital_types.h
add_subdirectory( plugin_management )
# -- Created library "vital_applets"
add_subdirectory( applets )

# ==============================================================================
# Built-in plugin modules
#
# This is specifically after the Vital library definition as some of these
# require the vital library for parts and utility.
# And, you know, the plugin management stuff (except for the logger, which works
# independently from the main plugin management so that it can have logging
# itself).
#
add_subdirectory( logger_plugins )
add_subdirectory( config_plugins )
add_subdirectory( applets_plugins )

if (KWIVER_ENABLE_TOOLS)
  add_subdirectory( tools )
endif()

if (KWIVER_ENABLE_SERIALIZE_PROTOBUF)
  add_subdirectory( types/protobuf )
endif()

###
# Add tests if enabled
if (KWIVER_ENABLE_TESTS)
  add_subdirectory( tests )
endif()

add_subdirectory( test_interface )
