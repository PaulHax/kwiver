###
# Top level build script for kwiver-build
#
cmake_minimum_required(VERSION 3.15)

project(KWIVER)

###
# KWIVER version
file( READ "VERSION.txt" KWIVER_VERSION )
string( STRIP "${KWIVER_VERSION}" KWIVER_VERSION )
string( REGEX MATCH "^([0-9]+)\.([0-9]+)\.([0-9]+)" _ "${KWIVER_VERSION}" )
set( KWIVER_VERSION_MAJOR "${CMAKE_MATCH_1}" )
set( KWIVER_VERSION_MINOR "${CMAKE_MATCH_2}" )
set( KWIVER_VERSION_PATCH "${CMAKE_MATCH_3}" )

# C++17 is required
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set default visibility
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

# Allow capitalized _ROOT variables
if(POLICY_CMP0144)
  cmake_policy(SET CMP0144 NEW)
endif()

# Organize target into folders for IDEs that support it
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

###
# Look for and use Fletch to find dependencies
#
set( fletch_DIR "" CACHE PATH "Path to FLETCH packages" )
if ( IS_DIRECTORY ${fletch_DIR} )
  find_package( fletch NO_MODULE )
  if ( fletch_FOUND )
    # Search for libraries in the fletch install root
    list(APPEND CMAKE_PREFIX_PATH "${fletch_ROOT}")
  endif()
endif()

###
# project specific directories
##
set(KWIVER_CMAKE_DIR            "${CMAKE_SOURCE_DIR}/CMake")

# root directories
set(KWIVER_SOURCE_DIR           "${CMAKE_SOURCE_DIR}")
set(KWIVER_BINARY_DIR           "${CMAKE_BINARY_DIR}")

# Default library dir
set(KWIVER_DEFAULT_LIBRARY_DIR "lib" CACHE STRING
    "Default library directory for kwiver")
mark_as_advanced(KWIVER_DEFAULT_LIBRARY_DIR)

#
# kwiver module config file.
set(KWIVER_CONFIG_FILE          "${KWIVER_BINARY_DIR}/kwiver-config.cmake")

if(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
  if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 19.30)
    set(_vcVersion vc17)
  elseif(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 19.20)
    set(_vcVersion vc16)
  elseif(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 19.10)
    set(_vcVersion vc15)
  elseif(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 19)
    set(_vcVersion vc14)
  else()
    message(FATAL_ERROR "KWIVER requires Visual Studio 2015 or greater")
  endif()
  # for pluggable macro magic to work we need a compliant preprocessor.
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:preprocessor")
endif()

# prepend our CMake resources
list(INSERT CMAKE_MODULE_PATH 0 ${KWIVER_CMAKE_DIR})

# Import other CMake modules that are back-ports from "future" versions.
# "Future" generally meaning versions of CMake that are greater than the one
# currently used.
include(kwiver-cmake-future)
option( KWIVER_ENABLE_PYTHON   "Enable python code" OFF)
if(KWIVER_ENABLE_PYTHON AND WIN32 AND SKBUILD)
  # When patching wheels on windows with delvewheel only pyd modules and
  # libraries are inspected for dependencies and are being patched.
  # So, we force .pyd extension to all of them to ensure they will be
  # picked up by delvewheel.
  # See also https://github.com/adang1345/delvewheel/issues/47
  set(CMAKE_SHARED_MODULE_SUFFIX ".pyd")
  set(CMAKE_SHARED_LIBRARY_SUFFIX ".pyd")
endif()

# ===========================================================================================
# project global includes
include_directories("${KWIVER_SOURCE_DIR}")
#xxx(kwiver-2) re-enable once ported
#include_directories("${KWIVER_SOURCE_DIR}/sprokit/src")
include_directories("${KWIVER_BINARY_DIR}")
#xxx(kwiver-2) re-enable once ported
#include_directories("${KWIVER_BINARY_DIR}/sprokit/src")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# include vital internal headers for private use only
include_directories("${KWIVER_SOURCE_DIR}/vital/internal")

# kwiversys includes are generated in the binary directory
include_directories("${KWIVER_BINARY_DIR}/vital/kwiversys")

# ===========================================================================================

include_directories( SYSTEM ${CMAKE_CURRENT_SOURCE_DIR} )

# ===========================================================================================
###
# User options
include(CMakeDependentOption)

# TODO: Add a comment documenting why we have a separate build-shared flag.
OPTION(KWIVER_BUILD_SHARED     "Build KWIVER components shared or not" TRUE )
MARK_AS_ADVANCED( KWIVER_BUILD_SHARED )
set(BUILD_SHARED_LIBS ${KWIVER_BUILD_SHARED})

OPTION(KWIVER_EXPORT_COMPILE_COMMANDS "Export compile commands to support LSP development." FALSE )
MARK_AS_ADVANCED( KWIVER_EXPORT_COMPILE_COMMANDS )
if (KWIVER_EXPORT_COMPILE_COMMANDS )
  set(CMAKE_EXPORT_COMPILE_COMMANDS ${KWIVER_EXPORT_COMPILE_COMMANDS})
endif()

# TODO: Enable-* flags will need clean-up with reorganization.
OPTION(KWIVER_ENABLE_TESTS                "Enable testing" OFF )
OPTION(KWIVER_ENABLE_ARROWS               "Enable building arrows" OFF )
OPTION(KWIVER_ENABLE_TOOLS                "Enable tools" OFF )
OPTION(KWIVER_ENABLE_TRACK_ORACLE         "Enable track_oracle" OFF )
OPTION(KWIVER_ENABLE_EXAMPLES             "Enable examples" OFF )

OPTION(KWIVER_ENABLE_SERIALIZE_PROTOBUF   "Enable protobuf serialization" OFF )
OPTION(KWIVER_ENABLE_SERIALIZE_JSON       "Enable json serialization" OFF )

# if sprokit enabled
OPTION(KWIVER_ENABLE_SPROKIT              "Enable building sprokit" OFF )

if(KWIVER_ENABLE_ARROWS)
  OPTION(KWIVER_ENABLE_GEOCALC            "Enable Geographic Calculation Arrow" ON )
  OPTION(KWIVER_ENABLE_MVG                "Enable Multi-View Geometry Arrow" ON )
  OPTION(KWIVER_ENABLE_KLV                "Enable Key-Length-Value Metadata Arrow" ON)
  OPTION(KWIVER_ENABLE_ZLIB               "Enable Zlib Arrow" ON)
endif()

CMAKE_DEPENDENT_OPTION(KWIVER_ENABLE_PROCESSES
  "Should the KWIVER Sprokit Processes Be Built?" OFF
  "KWIVER_ENABLE_ARROWS AND KWIVER_ENABLE_SPROKIT" OFF )

OPTION(KWIVER_USE_BUILD_TREE "Include build-tree plugin directory to the default plugin path list" OFF )
MARK_AS_ADVANCED(KWIVER_USE_BUILD_TREE)

option( KWIVER_ENABLE_EXTRAS "Enable extras content" OFF )


CMAKE_DEPENDENT_OPTION( KWIVER_ENABLE_PYTHON_TESTS "Enable python tests" ON
    "KWIVER_ENABLE_PYTHON; KWIVER_ENABLE_TESTS" OFF )

if (KWIVER_ENABLE_SERIALIZE_PROTOBUF)
  if (WIN32)
    message( WARNING "Protobuf serialization not supported for windows. Being disabled." )
    set(KWIVER_ENABLE_SERIALIZE_PROTOBUF OFF )
  endif()
endif()

# =========================================================================================
###
# KWIVER CMake config
#
include( kwiver-utils )

if (KWIVER_ENABLE_PYTHON)
  if (NOT WIN32)
    OPTION(KWIVER_SYMLINK_PYTHON "Symlink python files instead of copying." OFF)
  endif()
endif()

set(BUILD_SHARED_LIBS ${KWIVER_BUILD_SHARED})

# Set directories where loadable modules are stored.
# These subdirs are under .../lib/
set( kwiver_plugin_subdir                         kwiver/plugins )
set( kwiver_plugin_process_subdir                 ${kwiver_plugin_subdir}/processes )
set( kwiver_plugin_algorithm_subdir               ${kwiver_plugin_subdir}/algorithms )
set( kwiver_plugin_process_instrumentation_subdir ${kwiver_plugin_subdir}/processes )
set( kwiver_plugin_scheduler_subdir               ${kwiver_plugin_subdir}/processes )
set( kwiver_plugin_module_subdir                  ${kwiver_plugin_subdir}/modules )
set( kwiver_plugin_plugin_explorer_subdir         ${kwiver_plugin_subdir}/plugin_explorer )
set( kwiver_plugin_logger_subdir                  ${kwiver_plugin_subdir}/logger )
set( kwiver_plugin_applets_subdir                 ${kwiver_plugin_subdir}/applets )

# other well known subdirs
set( kwiver_config_subdir                         share/kwiver/${KWIVER_VERSION}/config)

##
# System specific compiler flags and support
include( kwiver-flags )

##
# check compiler support
include( kwiver-configcheck )

# set the name for our package export
set( kwiver_export_name      kwiver_exports )

###
# External Dependencies and Modules - Most Will be supplied by Fletch
include( kwiver-depends )
# TODO: devise something more clear than this pattern: dependencies are too far
#   detached from what requires them. This makes it very unclear what depends
#   on what. Easy to create wild global flags.

###
# Configure setup scripts
include( kwiver-setup-scripts )

# define which boost we are using
if (DEFINED BOOST_ROOT)
  set(BOOST_ROOT_ARG ${BOOST_ROOT})
else()
  set(BOOST_ROOT_ARG ${Boost_LIBRARY_DIR}/..)
endif()

add_definitions(-DKWIVER_DEFAULT_PLUGIN_ORGANIZATION="Kitware Inc.")

# This regex stuff is to support CentOS with broken std::regex
# - VITAL_USE_STD_REGEX is the product of a compile check, see
#   ``CMake/kwiver-configcheck.cmake`` ~L68.
# - This was introduced for being able to CXXOPTS, found in ``vital/applets/``.
if ( NOT VITAL_USE_STD_REGEX )
  set( VITAL_BOOST_REGEX ${Boost_REGEX_LIBRARY})
  add_definitions( -DKWIVER_USE_BOOST_REGEX )
endif()


# ===========================================================================================
link_directories( ${KWIVER_BINARY_DIR}/lib )

# ===========================================================================================
###
# Add tests if enabled
if (KWIVER_ENABLE_TESTS)
  enable_testing()
  include(CTest)
  set(BUILD_TESTING    ON)  # needed for compatibility with kwiversys
  mark_as_advanced(BUILD_TESTING)

  configure_file(
    "${KWIVER_CMAKE_DIR}/CTestCustom.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/CTestCustom.cmake"
    @ONLY)

  add_subdirectory( tests )
  include_directories( tests )
endif()

if (NOT IS_ABSOLUTE "${CMAKE_INSTALL_PREFIX}")
  message(FATAL_ERROR "KWIVER does not support a relative install prefix "
    "(${CMAKE_INSTALL_PREFIX}).")
endif ()

# =============================================================================================

set( LIB_SUFFIX "" CACHE STRING
  "Library directory suffix. e.g. suffix=\"kwiver\" will install libraries in \"libkwiver\" rather than \"lib\"")
mark_as_advanced( LIB_SUFFIX )
# Descend into project directories noq.

if (KWIVER_ENABLE_TRACK_ORACLE)
  add_subdirectory( track_oracle )
endif()

if (KWIVER_ENABLE_ARROWS)
  add_subdirectory( arrows )
endif()

if (KWIVER_ENABLE_SPROKIT)
  add_subdirectory(sprokit)
endif()

if (KWIVER_ENABLE_EXTRAS)
  add_subdirectory(extras)
endif()

if (KWIVER_ENABLE_EXAMPLES)
  add_subdirectory(examples)
endif()

if (KWIVER_ENABLE_TOOLS)
  add_subdirectory(tools)
endif()

if (KWIVER_ENABLE_PYTHON)
  add_subdirectory(python)
endif()
###
# Descend into project directories now.
# include as the last subdir
add_subdirectory( vital )

# this is where the algorithm default configuration files live
add_subdirectory(config)

###
# Generate vital/version.h. This file is needed by sprokit so must be generated here, before sprokit is included
#
# Use Git (if available) to add Git hash info to the version header
set(kwiver_configure_with_git on)
kwiver_configure_file(version.h
  "${CMAKE_CURRENT_SOURCE_DIR}/vital/version.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/vital/version.h"
  KWIVER_VERSION_MAJOR
  KWIVER_VERSION_MINOR
  KWIVER_VERSION_PATCH
  KWIVER_VERSION
  KWIVER_SOURCE_DIR
  )
set(kwiver_configure_with_git)

if(MSVC)
  # Generate files to inform msvc of the kwiver runtime environment
  kwiver_setup_msvc_env() # Setup .bat scripts need to be complete and on disk
endif()

#############################################################################################
# Marking source root for documentation generation
#
kwiver_create_doxygen( kwiver "${CMAKE_CURRENT_LIST_DIR}" )
kwiver_create_sphinx()


#############################################################################################
# Top level installation
#
set(kwiver_cmake_install_dir lib${LIB_SUFFIX}/cmake/kwiver)

# Install rules for CMake utilities
include( kwiver-install-utils )

# Prepare space-separated list of library names for config
get_property(kwiver_libs GLOBAL PROPERTY kwiver_libraries)
string(REPLACE ";" " " kwiver_libs "${kwiver_libs}")

# setup general paths to includes and libs for packages we use/provide
set( KWIVER_INCLUDE_DIRS "")
set( KWIVER_LIBRARY_DIRS "")

# TODO: use global property to capture found, external-to-fletch eigen dep
list(APPEND KWIVER_INCLUDE_DIRS "${EIGEN3_INCLUDE_DIR}")

# TODO: use global property to capture found, external-to-fletch boost dep
if ( NOT fletch_ENABLED_Boost)
  # define which boost we are using
  if (DEFINED BOOST_ROOT_ARG)
    list( APPEND KWIVER_INCLUDE_DIRS "${BOOST_ROOT_ARG}/include")
    list( APPEND KWIVER_LIBRARY_DIRS "${BOOST_ROOT_ARG}/lib")
  endif()
endif()

if (fletch_FOUND)
  # since we have fletch, add include and lib dirs to export
  list( APPEND KWIVER_INCLUDE_DIRS "${fletch_ROOT}/include")
  list( APPEND KWIVER_LIBRARY_DIRS "${fletch_ROOT}/lib")
endif()

configure_file(
  "${KWIVER_SOURCE_DIR}/CMake/kwiver-config-build.cmake.in"
  "${KWIVER_CONFIG_FILE}"
  @ONLY
  )

# Configure install-tree CMake config file and export associated targets file
set(KWIVER_CONFIG_INSTALL_FILE "${KWIVER_BINARY_DIR}/kwiver-config-install.cmake")
configure_file(
  "${KWIVER_SOURCE_DIR}/CMake/kwiver-config-install.cmake.in"
  "${KWIVER_CONFIG_INSTALL_FILE}"
  @ONLY
  )

kwiver_export_targets("${KWIVER_BINARY_DIR}/kwiver-config-targets.cmake")

if(NOT SKBUILD)
  kwiver_install(
    FILES       "${KWIVER_CONFIG_INSTALL_FILE}"
    DESTINATION "${kwiver_cmake_install_dir}"
    RENAME      kwiver-config.cmake
    )

  kwiver_install(
    EXPORT      ${kwiver_export_name}
    NAMESPACE   kwiver::
    DESTINATION "${kwiver_cmake_install_dir}"
    FILE        kwiver-config-targets.cmake
    )
endif()
###
# link compile commands to root of source tree
if ( CMAKE_EXPORT_COMPILE_COMMANDS )
  # link file to root of source tree
  kwiver_make_symlink( ${CMAKE_BINARY_DIR}/compile_commands.json
                       ${CMAKE_SOURCE_DIR}/compile_commands.json )
endif()

# --------------------------------------------------------------------------------------
# Configure header file defining plugin path defaults.
#
# Build a default set of plugin path dirs
# Provide that list as the default value for the path option.
#
# By this point, all eligible paths should have been added to the kwiver_plugin_path
# property via calls to the `kwiver_add_module_path` cmake function.
#
# This is here at the project level because it is currently assumed that more
# than VITAL will add paths.
#
# TODO: Revisit the above assumption.
#       If no longer true, this may be moved into the vital sub-project.
#
if (WIN32)
  set(path_sep "\\073")
else()  # Other Unix systems
  set(path_sep "\\072")
endif()

set(KWIVER_DEFAULT_MODULE_PATHS ""
  CACHE STRING "The default paths for module scanning. Separate paths with ';' character." FORCE)
mark_as_advanced( KWIVER_DEFAULT_MODULE_PATHS )

# add all user supplied paths to the property
foreach( p IN LISTS KWIVER_DEFAULT_MODULE_PATHS )
  kwiver_add_module_path( ${p} )
endforeach(p)

kwiver_make_module_path( ${CMAKE_INSTALL_PREFIX} ${kwiver_plugin_subdir} )
kwiver_add_module_path(  "${kwiver_module_path_result}" )

set(sprokit_default_pipe_include_paths
  "${CMAKE_INSTALL_PREFIX}/${kwiver_config_subdir}/pipelines/include")

set(sprokit_default_cluster_paths
  "${CMAKE_INSTALL_PREFIX}/${kwiver_config_subdir}/pipelines/clusters")

if (KWIVER_USE_BUILD_TREE)
  kwiver_make_module_path( ${KWIVER_BINARY_DIR} ${kwiver_plugin_subdir} )
  kwiver_add_module_path(  "${kwiver_module_path_result}" )

  set(sprokit_default_pipe_include_paths
    "${sprokit_default_pipe_include_paths}${path_sep}${KWIVER_SOURCE_DIR}/config/pipelines")

  set(sprokit_default_cluster_paths
    "${sprokit_default_cluster_paths}${path_sep}${KWIVER_SOURCE_DIR}/config/clusters")
endif()

set(SPROKIT_DEFAULT_PIPE_INCLUDE_PATHS "${sprokit_default_pipe_include_paths}"
  CACHE STRING "The default paths to search for pipe includes in" FORCE)

set(SPROKIT_DEFAULT_CLUSTER_PATHS "${sprokit_default_cluster_paths}"
  CACHE STRING "The default paths to search for clusters in" FORCE)

# need to retrieve the GLOBAL PROPERTY kwiver_plugin_path and
# formulate the default module path
get_property(plugin_path GLOBAL PROPERTY kwiver_plugin_path)

# add all user configured paths to our list
foreach( p IN LISTS plugin_path )
  if ( VITAL_MODULE_PATH )
    set( VITAL_MODULE_PATH "${VITAL_MODULE_PATH}${path_sep}${p}" )
  else()
    set( VITAL_MODULE_PATH "${p}" )
  endif()
endforeach(p)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/vital/kwiver-include-paths.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/vital/kwiver-include-paths.h" )
# END -- possible move to vital sub-directory

# ===========================================================================================
# CPack Packaging
#

#TODO: Define package dependencies
set(KWIVER_DEPS "")

if(EXISTS /etc/redhat-release)
  file(READ /etc/redhat-release RHEL_VERSION)
  string(REGEX REPLACE ".*release ([^\\. ]*).*" "\\1" RHEL_VERSION "${RHEL_VERSION}")
  set(CPACK_SYSTEM_NAME "el${RHEL_VERSION}.${CMAKE_SYSTEM_PROCESSOR}")
  set(CPACK_RPM_PACKAGE_AUTOREQPROV " no")
  set(CPACK_RPM_PACKAGE_REQUIRES "${KWIVER_DEPS}")
else()
  set(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
endif()

set(CPACK_PACKAGE_NAME              "KWIVER")
set(CPACK_PACKAGE_VENDOR            "Kitware, Inc.")
set(CPACK_PACKAGE_CONTACT           "kitware@kitware.com")
set(CPACK_MONOLITHIC_INSTALL        true)
set(CPACK_PACKAGE_VERSION_MAJOR     "${KWIVER_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR     "${KWIVER_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH     "${KWIVER_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION           "${KWIVER_VERSION}")
set(CPACK_RESOURCE_FILE_LICENSE     "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGING_INSTALL_PREFIX  "${CMAKE_INSTALL_PREFIX}")
set(CPACK_PACKAGE_FILE_NAME         "${CPACK_PACKAGE_NAME}-"
                                    "${CPACK_PACKAGE_VERSION}-"
                                    "${CPACK_SYSTEM_NAME}")

include (CPack)
