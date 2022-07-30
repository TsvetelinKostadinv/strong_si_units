cmake_minimum_required(VERSION 3.15)

project(SI_units CXX)

option(ENABLE_SANDBOX "Should the sandbox project be included" ON)
option(ENABLE_TESTS "Should the test project be included" OFF)
option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" ON)
option(ENABLE_CLANG_TIDY "Enable static analysis with clang-tidy" OFF)
option(ENABLE_SI_CONSTANTS "The library provides big_int definitions for the SI constants" OFF)

set(MSVC_WARNINGS
  /W4 # Baseline reasonable warnings
  /w14242 # 'identifier': conversion from 'type1' to 'type1', possible loss of data
  /w14254 # 'operator': conversion from 'type1:field_bits' to 'type2:field_bits', possible loss of data
  /w14263 # 'function': member function does not override any base class virtual member function
  /w14265 # 'classname': class has virtual functions, but destructor is not virtual instances of this class may not
          # be destructed correctly
  /w14287 # 'operator': unsigned/negative constant mismatch
  /we4289 # nonstandard extension used: 'variable': loop control variable declared in the for-loop is used outside
          # the for-loop scope
  /w14296 # 'operator': expression is always 'boolean_value'
  /w14311 # 'variable': pointer truncation from 'type1' to 'type2'
  /w14545 # expression before comma evaluates to a function which is missing an argument list
  /w14546 # function call before comma missing argument list
  /w14547 # 'operator': operator before comma has no effect; expected operator with side-effect
  /w14549 # 'operator': operator before comma has no effect; did you intend 'operator'?
  /w14555 # expression has no effect; expected expression with side- effect
  /w14619 # pragma warning: there is no warning number 'number'
  /w14640 # Enable warning on thread un-safe static member initialization
  /w14826 # Conversion from 'type1' to 'type_2' is sign-extended. This may cause unexpected runtime behavior.
  /w14905 # wide string literal cast to 'LPSTR'
  /w14906 # string literal cast to 'LPWSTR'
  /w14928 # illegal copy-initialization; more than one user-defined conversion has been implicitly applied
  /permissive- # standards conformance mode for MSVC compiler.
)

if(WARNINGS_AS_ERRORS)
  set(MSVC_WARNINGS ${MSVC_WARNINGS} /WX)
endif()

if(MSVC)
  set(PROJECT_WARNINGS ${MSVC_WARNINGS})
else()
  message(SEND_ERROR "The compile flags are set only for MSVC")
endif()

if(MSVC)
  set(CONSTEXPR_OP_LIMIT /constexpr:steps10000000)
endif()

set(PROJECT_SETTINGS ${MSVC_WARNINGS} ${CONSTEXPR_OP_LIMIT})

if(ENABLE_SI_CONSTANTS)
  add_compile_definitions(DEFINE_SI_CONSTANTS)
endif()

if(ENABLE_CLANG_TIDY)
  find_program(CLANGTIDY clang-tidy)
  if(CLANGTIDY)
    set(CMAKE_CXX_CLANG_TIDY ${CLANGTIDY} -extra-arg=-Wno-unknown-warning-option)
    if(WARNINGS_AS_ERRORS)
      list(APPEND CMAKE_CXX_CLANG_TIDY -warnings-as-errors=*)
    endif()
  else()
    message(SEND_ERROR "clang-tidy requested but executable not found")
  endif()
endif()

# add_subdirectory(src)
# target_compile_options(SI_units INTERFACE ${PROJECT_WARNINGS})

add_subdirectory(sandbox)
target_compile_options(sandbox PRIVATE ${PROJECT_SETTINGS})

if(ENABLE_TESTING)
  enable_testing()
  add_subdirectory(test)
  add_compile_options(test INTERFACE ${PROJECT_SETTINGS})
endif()