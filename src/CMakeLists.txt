set(SOURCES
  big_int.cpp
)

set(HEADERS
  ../include/big_int/big_int.hpp
)

add_library(big_int STATIC ${SOURCES} ${HEADERS})
target_compile_features(big_int PUBLIC cxx_std_17)

target_include_directories(big_int PRIVATE ../include/big_int)