include(CTest)
include(Catch)

set(COMMON_HEADERS
  ../include/big_int/big_int.hpp
  ../include/util.hpp
  ../include/si_constants.hpp
  ../include/big_int/big_int_std_integration.hpp
  ../include/big_int/big_int_util.hpp
)

set(RUNTIME_TEST_SOURCES
  test.cpp
)
add_executable(runtime_tests ${RUNTIMRUNTIME_TEST_SOURCESE_SOURCES} ${COMMON_HEADERS})
target_link_libraries(runtime_tests PRIVATE Catch2::Catch2WithMain)

catch_discover_tests(runtime_tests)

set(CONSTEXPR_TEST_SOURCES
  constexpr_test.cpp
)
add_executable(constexpr_tests ${CONSTEXPR_TEST_SOURCES} ${COMMON_HEADERS})
target_link_libraries(constexpr_tests PRIVATE Catch2::Catch2WithMain)

catch_discover_tests(constexpr_tests)

add_executable(relaxed_constexpr_tests ${CONSTEXPR_TEST_SOURCES} ${COMMON_HEADERS})
target_link_libraries(relaxed_constexpr_tests PRIVATE Catch2::Catch2WithMain)

add_compile_definitions(CATCH_CONFIG_RUNTIME_STATIC_REQUIRE)

catch_discover_tests(relaxed_constexpr_tests)
