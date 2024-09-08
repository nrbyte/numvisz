# Barchart executable configuration
add_executable(numvisz_linechartrace)

target_sources(numvisz_linechartrace PRIVATE
  src/main.cpp
  src/application.cpp
  src/application.hpp
)

target_include_directories(numvisz_linechartrace PRIVATE src)

# Include the viszbase library
target_link_libraries(numvisz_linechartrace viszbase)

