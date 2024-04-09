set(CMAKE_PREFIX_PATH "$ENV{CI_PROJECT_DIR}/.gitlab/fletch" CACHE STRING "")

include("${CMAKE_CURRENT_LIST_DIR}/configure_common.cmake")
