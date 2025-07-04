include("${CMAKE_CURRENT_LIST_DIR}/gitlab_ci.cmake")

# Read the files from the build directory.
ctest_read_custom_files("${CTEST_BINARY_DIRECTORY}")

# Pick up from where the configure left off.
ctest_start(APPEND)

include(ProcessorCount)
ProcessorCount(nproc)
if (NOT "$ENV{CTEST_MAX_PARALLELISM}" STREQUAL "")
  if (nproc GREATER "$ENV{CTEST_MAX_PARALLELISM}")
    set(nproc "$ENV{CTEST_MAX_PARALLELISM}")
  endif ()
endif ()

if (CTEST_CMAKE_GENERATOR STREQUAL "Unix Makefiles")
  set(CTEST_BUILD_FLAGS "-j${nproc} -l${nproc}")
elseif (CTEST_CMAKE_GENERATOR MATCHES "Ninja")
  set(CTEST_BUILD_FLAGS "-l${nproc}")
endif ()

if (CTEST_CMAKE_GENERATOR MATCHES "Make")
  # Drop the `-i` flag without remorse.
  set(CTEST_BUILD_COMMAND "make ${CTEST_BUILD_FLAGS}")
endif ()

ctest_build(
  NUMBER_ERRORS   num_errors
  NUMBER_WARNINGS num_warnings
  RETURN_VALUE    build_result)

ctest_submit(PARTS Build)

if (build_result)
  file(GLOB logs
    "${CTEST_SOURCE_DIRECTORY}/compile_output.log")
  if (logs)
    list(APPEND CTEST_NOTES_FILES ${logs})
    ctest_submit(PARTS Notes)
  endif ()
endif ()

include("${CMAKE_CURRENT_LIST_DIR}/ctest_annotation.cmake")
if (DEFINED build_id)
  ctest_annotation_report("${CTEST_BINARY_DIRECTORY}/annotations.json"
      "Build Errors (${num_errors})" "https://open.cdash.org/viewBuildError.php?buildid=${build_id}"
      "Build Warnings (${num_warnings})" "https://open.cdash.org/viewBuildError.php?type=1&buildid=${build_id}"
  )
endif ()

if (build_result)
  message(FATAL_ERROR
    "Failed to build")
endif ()

file(WRITE "${CTEST_SOURCE_DIRECTORY}/compile_num_warnings.log" "${num_warnings}")

if ("$ENV{CTEST_NO_WARNINGS_ALLOWED}" AND num_warnings GREATER 0)
  message(FATAL_ERROR
    "Found ${num_warnings} warnings (treating as fatal).")
endif ()
