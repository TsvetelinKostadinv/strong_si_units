set(SOURCES
  main.cpp
)

set(HEADERS
  ../include/big_int/big_int.hpp
)

add_executable(sandbox ${SOURCES} ${HEADERS})
target_compile_features(sandbox PUBLIC cxx_std_17)

if(MSVC)
	set(CONSTEXPR_OP_LIMIT /constexpr:steps10000000)
endif()

target_compile_options(sandbox PRIVATE ${CONSTEXPR_OP_LIMIT})

target_include_directories(sandbox PRIVATE ../include/big_int)