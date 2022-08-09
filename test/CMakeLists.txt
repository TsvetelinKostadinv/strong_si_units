include(CTest)
include(Catch)

set(COMMON_HEADERS
  ../include/big_int/big_int.hpp
  ../include/util.hpp
  ../include/si_constants.hpp
  ../include/big_int/big_int_std_integration.hpp
  ../include/big_int/big_int_util.hpp
  test_util.hpp
)

set(RUNTIME_TEST_SOURCES
  test.cpp
)
add_executable(runtime_tests ${RUNTIME_TEST_SOURCES} ${COMMON_HEADERS})
target_link_libraries(runtime_tests PRIVATE project_warnings project_options)
target_link_libraries(runtime_tests PRIVATE Catch2::Catch2WithMain)

catch_discover_tests(runtime_tests)

set(CONSTEXPR_TEST_SOURCES
  constexpr_test.cpp
)
add_executable(constexpr_tests ${CONSTEXPR_TEST_SOURCES} ${COMMON_HEADERS})
target_link_libraries(constexpr_tests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(constexpr_tests PRIVATE project_warnings project_options)

catch_discover_tests(constexpr_tests)

add_executable(relaxed_constexpr_tests ${CONSTEXPR_TEST_SOURCES} ${COMMON_HEADERS})
target_link_libraries(relaxed_constexpr_tests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(relaxed_constexpr_tests PRIVATE project_warnings project_options)

add_compile_definitions(CATCH_CONFIG_RUNTIME_STATIC_REQUIRE)

catch_discover_tests(relaxed_constexpr_tests)

target_include_directories(runtime_tests PRIVATE ../include/big_int)

add_custom_target(ALL_TESTS COMMAND ${CMAKE_CTEST_COMMAND})
add_dependencies(
  ALL_TESTS
  runtime_tests
  constexpr_tests
  relaxed_constexpr_tests
  Catch2::Catch2WithMain
  Catch2::Catch2
)

set_target_properties(
  runtime_tests
  constexpr_tests
  relaxed_constexpr_tests
  ALL_TESTS
  Catch2WithMain
  Catch2
  PROPERTIES
  FOLDER "Tests"
  EXCLUDE_FROM_ALL TRUE
)