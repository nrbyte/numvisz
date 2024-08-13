# Barchart executable configuration
add_executable(barchart)

target_sources(barchart PRIVATE
  src/main.cpp
  src/application.cpp
  src/application.hpp
  src/math.cpp
  src/math.hpp
  src/gui.cpp
  src/gui.hpp
  src/shader.cpp
  src/shader.hpp
  src/renderer.cpp
  src/renderer.hpp
  src/fontrenderer.cpp
  src/fontrenderer.hpp
  src/csvparser.cpp
  src/csvparser.hpp
  src/commandlineparser.cpp
  src/commandlineparser.hpp
  src/timer.cpp
  src/timer.hpp
)

# For the glad folder
target_include_directories(barchart PRIVATE src)

# For shaders
target_include_directories(barchart PRIVATE .)

# Find and include GLFW
find_package(glfw3)
if (NOT TARGET glfw)
  message(FATAL_ERROR "GLFW not found on system! You can use vcpkg (JSON manifest already provided), or provide the dependency another way.")
endif()
target_link_libraries(barchart glfw)

# Find and include freetype
find_package(Freetype)
if (NOT ${FREETYPE_FOUND})
  message(FATAL_ERROR "Freetype not found on system! You can use vcpkg (JSON manifest already provided), or provide the dependency another way.")
endif()
target_link_libraries(barchart ${FREETYPE_LIBRARIES})
target_include_directories(barchart PRIVATE ${FREETYPE_INCLUDE_DIRS})

# Find and include OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(barchart OpenGL::GL)

