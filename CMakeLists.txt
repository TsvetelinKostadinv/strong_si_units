cmake_minimum_required(VERSION 3.15)

project(SI_units CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Options
option(ENABLE_SANDBOX "Should the sandbox project be included" ON)
option(ENABLE_TESTING "Should the test project be included" OFF)
option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" ON)
option(ENABLE_CLANG_TIDY "Enable static analysis with clang-tidy" OFF)
option(ENABLE_SI_CONSTANTS "The library provides big_int definitions for the SI constants" OFF)
option(ENABLE_BIG_INT_STD_INTEGRATION "Enable integration with C++ standard library" OFF)
option(ENABLE_BIG_INT_UTIL "Enable utilities for the big_int" OFF)
option(ENABLE_BIG_INT_LITERAL "Enable the custom compile time literal for big_int" OFF)

# Warnings
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

set(CLANG_WARNINGS
  -Wall
  -Wextra                # reasonable and standard
  -Wshadow               # warn the user if a variable declaration shadows one from a parent context
  -Wnon-virtual-dtor     # warn the user if a class with virtual functions has a non-virtual destructor. This helps
                         # catch hard to track down memory errors
  -Wold-style-cast       # warn for c-style casts
  -Wcast-align           # warn for potential performance problem casts
  -Wunused               # warn on anything being unused
  -Woverloaded-virtual   # warn if you overload (not override) a virtual function
  -Wpedantic             # warn if non-standard C++ is used
  -Wconversion           # warn on type conversions that may lose data
  -Wsign-conversion      # warn on sign conversions
  -Wnull-dereference     # warn if a null dereference is detected
  -Wdouble-promotion     # warn if float is implicit promoted to double
  -Wformat=2             # warn on security issues around functions that format output (ie printf)
  -Wimplicit-fallthrough # warn on statements that fallthrough without an explicit annotation
  -Wno-unknown-pragmas
)

set(GCC_WARNINGS
  ${CLANG_WARNINGS}
  -Wmisleading-indentation    # warn if indentation implies blocks where blocks do not exist
  -Wduplicated-cond           # warn if if / else chain has duplicated conditions
  -Wduplicated-branches       # warn if if / else branches have duplicated code
  -Wlogical-op                # warn about logical operations being used where bitwise were probably wanted
  -Wuseless-cast              # warn if you perform a cast to the same type
)

if(WARNINGS_AS_ERRORS)
  message(TRACE "Warnings are treated as errors")
  list(APPEND CLANG_WARNINGS -Werror)
  list(APPEND GCC_WARNINGS -Werror)
  list(APPEND MSVC_WARNINGS /WX)
endif()

# Constexpr steps
set(MAX_CONSTEXPR_STEPS 10000000)

if(MSVC)
  list(APPEND MSVC_WARNINGS /constexpr:steps${MAX_CONSTEXPR_STEPS})
elseif(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
  list(APPEND CLANG_WARNINGS -fconstexpr-steps=${MAX_CONSTEXPR_STEPS})
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  list(APPEND GCC_WARNINGS -fconstexpr-ops-limit=${MAX_CONSTEXPR_STEPS})
else()
  message(AUTHOR_WARNING "No compiler warnings set for CXX compiler: '${CMAKE_CXX_COMPILER_ID}'")
  # TODO support Intel compiler
endif()

# Set project warnings
if(MSVC)
  set(PROJECT_WARNINGS_CXX ${MSVC_WARNINGS})
elseif(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
  set(PROJECT_WARNINGS_CXX ${CLANG_WARNINGS})
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(PROJECT_WARNINGS_CXX ${GCC_WARNINGS})
else()
  message(AUTHOR_WARNING "No compiler warnings set for CXX compiler: '${CMAKE_CXX_COMPILER_ID}'")
  # TODO support Intel compiler
endif()

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
target_compile_options(sandbox PRIVATE ${PROJECT_WARNINGS_CXX})

if(ENABLE_TESTING)
  add_subdirectory(lib/catch2)
  enable_testing()

  include(CTest)
  include(Catch)

  add_subdirectory(test)
  add_compile_options(tests PRIVATE ${PROJECT_WARNINGS_CXX})
endif()