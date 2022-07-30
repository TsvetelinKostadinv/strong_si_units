set(SOURCES
  main.cpp
)

set(HEADERS
  ../include/big_int/big_int.hpp
  ../include/util.hpp
  ../include/si_constants.hpp
)

add_executable(sandbox ${SOURCES} ${HEADERS})
target_compile_features(sandbox PUBLIC cxx_std_17)

target_include_directories(sandbox PRIVATE ../include/big_int)