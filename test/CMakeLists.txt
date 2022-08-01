# add_directory(../lib/catch2)
# list(APPEND CMAKE_MODULE_PATH ../lib/catch2/CMake/)

# find_package(Catch2 3 REQUIRED)

set(SOURCES
  test.cpp
)

set(HEADERS
  ../include/big_int/big_int.hpp
  ../include/util.hpp
  ../include/si_constants.hpp
  ../include/big_int/big_int_std_integration.hpp
  ../include/big_int/big_int_util.hpp
)

add_executable(tests ${SOURCES} ${HEADERS})
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

catch_discover_tests(tests)