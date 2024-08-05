cmake_minimum_required(VERSION 3.25)

project(dvisz VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)

# Useful for language servers
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Output all binaries in one place
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add barchart executable to this project
add_subdirectory(barchart)
