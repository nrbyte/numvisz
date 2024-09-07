cmake_minimum_required(VERSION 3.23)

project(numvisz VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)

# Useful for language servers
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Output all binaries in one place
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add the viszbase library to this project
add_subdirectory(base)
# Add barchart executable to this project
add_subdirectory(barchartrace)
# Add the UI to this project
add_subdirectory(picker)
