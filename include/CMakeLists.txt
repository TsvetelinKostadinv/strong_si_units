set(HEADERS
  si_constants.hpp
  util.hpp
  big_int/big_int.hpp
  big_int/big_int_std_integration.hpp
  big_int/big_int_util.hpp
  strong_type/strong_type.hpp
  strong_type/strong_decorators.hpp
)

add_library(si_lib INTERFACE)
target_sources(si_lib PUBLIC ${HEADERS})

# I don't want to force people to have all the warnings turned on
# target_link_libraries(si_lib PRIVATE project_options project_warnings)
target_include_directories(si_lib
  INTERFACE
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/include/big_int
  ${CMAKE_SOURCE_DIR}/include/strong_type
)