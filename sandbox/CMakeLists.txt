set(SOURCES
  main.cpp
)

set(HEADERS
  ../include/big_int/big_int.hpp
  ../include/util.hpp
  ../include/si_constants.hpp
  ../include/big_int/big_int_std_integration.hpp
  ../include/big_int/big_int_util.hpp
)

add_executable(sandbox ${SOURCES} ${HEADERS})

target_include_directories(sandbox PRIVATE ../include/big_int)