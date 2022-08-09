cmake_minimum_required(VERSION 3.15)

project(SI_units CXX)

include(${CMAKE_SOURCE_DIR}/cmake/feature_switches.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/project_options.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/project_warnings.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/sanitizers.cmake)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(sandbox)

if(ENABLE_TESTING)
  add_subdirectory(lib/catch2)
  enable_testing()

  add_subdirectory(test)
endif()