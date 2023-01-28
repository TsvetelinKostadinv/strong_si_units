set(SOURCES
  main.cpp
)

set(HEADERS
  
)

add_executable(sandbox ${SOURCES} ${HEADERS})
target_link_libraries(sandbox PRIVATE project_warnings project_options si_lib)