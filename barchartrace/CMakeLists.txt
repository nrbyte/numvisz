# Barchart executable configuration
add_executable(numvisz_barchartrace)

target_sources(numvisz_barchartrace PRIVATE
  src/main.cpp
  src/application.cpp
  src/application.hpp

  src/barchart.cpp
  src/barchart.hpp
)

target_include_directories(numvisz_barchartrace PRIVATE src)

# Include the viszbase library
target_link_libraries(numvisz_barchartrace viszbase)

