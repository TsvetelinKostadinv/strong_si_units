# Options
option(ENABLE_SANDBOX "Should the sandbox project be included" ON)
option(ENABLE_TESTING "Should the test project be included" OFF)
option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" ON)
option(ENABLE_CLANG_TIDY "Enable static analysis with clang-tidy" OFF)
option(ENABLE_SI_CONSTANTS "The library provides big_int definitions for the SI constants" OFF)
option(ENABLE_BIG_INT_STD_INTEGRATION "Enable integration with C++ standard library" OFF)
option(ENABLE_BIG_INT_UTIL "Enable utilities for the big_int" OFF)
option(ENABLE_BIG_INT_LITERAL "Enable the custom compile time literal for big_int" OFF)

if(ENABLE_SI_CONSTANTS)
  add_compile_definitions(DEFINE_SI_CONSTANTS)
endif()

if(ENABLE_BIG_INT_STD_INTEGRATION)
  add_compile_definitions(ENABLE_BIG_INT_STD_INTEGRATION)
endif()

if(ENABLE_BIG_INT_UTIL)
  add_compile_definitions(ENABLE_BIG_INT_UTIL)
endif()

if(ENABLE_BIG_INT_LITERAL)
  add_compile_definitions(ENABLE_BIG_INT_LITERAL)
endif()
