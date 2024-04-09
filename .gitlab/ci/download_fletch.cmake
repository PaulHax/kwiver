cmake_minimum_required(VERSION 3.12)

set(fletch_version "v20240404.0")
set(fletch_commit "c310f3cee87448a31ffbbb8f24c4bdd828bae071")

# Determine the tarball to download. ci-smtk-ci-developer-{date}-{git-sha}.tar.gz
# 20231127: Enable Python in bundles
if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "vs2022")
  set(fletch_checksum "e8bac0bab42377ff65dca02fde31dae6df3dfd46fc1699d2ace29c409a237f4f")
  set(fletch_platform "windows-x86_64")
  set(fletch_ext "zip")
else ()
  message(FATAL_ERROR
    "Unknown build to use for the fletch")
endif ()

# Ensure we have a hash to verify.
if (NOT DEFINED fletch_platform)
  message(FATAL_ERROR
    "Unknown platform for fletch")
endif ()
if (NOT DEFINED fletch_ext)
  message(FATAL_ERROR
    "Unknown extension for fletch")
endif ()
if (NOT DEFINED fletch_checksum)
  message(FATAL_ERROR
    "Unknown checksum for fletch")
endif ()

# Download the file.
file(DOWNLOAD
  "https://gitlab.kitware.com/api/v4/projects/6955/packages/generic/fletch-kwiver/${fletch_version}/fletch-kwiver-${fletch_commit}-${fletch_platform}.${fletch_ext}"
  ".gitlab/fletch.${fletch_ext}"
  STATUS download_status
  EXPECTED_HASH "SHA256=${fletch_checksum}")

# Check the download status.
list(GET download_status 0 res)
if (res)
  list(GET download_status 1 err)
  message(FATAL_ERROR
    "Failed to download fletch.${fletch_ext}: ${err}")
endif ()

# Extract the file.
execute_process(
  COMMAND
    "${CMAKE_COMMAND}"
    -E tar
    xf "fletch.${fletch_ext}"
  WORKING_DIRECTORY ".gitlab"
  RESULT_VARIABLE res
  ERROR_VARIABLE err
  ERROR_STRIP_TRAILING_WHITESPACE)
if (res)
  message(FATAL_ERROR
    "Failed to extract fletch.${fletch_ext}: ${err}")
endif ()
