
set(PROJECT_OPTIONS "")

# Constexpr steps
set(MAX_CONSTEXPR_STEPS 100000000)

if(MSVC)
  list(APPEND PROJECT_OPTIONS /constexpr:steps${MAX_CONSTEXPR_STEPS})
elseif(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
  list(APPEND PROJECT_OPTIONS -fconstexpr-steps=${MAX_CONSTEXPR_STEPS})
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  list(APPEND PROJECT_OPTIONS -fconstexpr-ops-limit=${MAX_CONSTEXPR_STEPS})
else()
  message(AUTHOR_WARNING "No compiler warnings set for CXX compiler: '${CMAKE_CXX_COMPILER_ID}'")

  # TODO support Intel compiler
endif()

add_library(project_options INTERFACE)
target_compile_options(project_options INTERFACE ${PROJECT_OPTIONS})