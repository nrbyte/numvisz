if (WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

add_library(viszbase SHARED)

target_sources(viszbase PRIVATE
  src/math.cpp
  include/viszbase/math.hpp
  src/gui.cpp
  include/viszbase/gui.hpp
  src/shader.cpp
  include/viszbase/shader.hpp
  src/renderer.cpp
  include/viszbase/renderer.hpp
  src/fontrenderer.cpp
  include/viszbase/fontrenderer.hpp
  src/csvparser.cpp
  include/viszbase/csvparser.hpp
  src/commandlineparser.cpp
  include/viszbase/commandlineparser.hpp
  src/timer.cpp
  include/viszbase/timer.hpp

  include/viszbase/color.hpp
)

target_include_directories(viszbase PUBLIC include)
# For the shaders
target_include_directories(viszbase PRIVATE .)

# Find and include OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(viszbase PUBLIC OpenGL::GL)

# Find and include GLFW
find_package(glfw3)
if (NOT TARGET glfw)
  message(FATAL_ERROR "GLFW not found on system! You can use vcpkg (JSON manifest already provided), or provide the dependency another way.")
endif()
target_link_libraries(viszbase PUBLIC glfw)

# Find and include freetype
find_package(Freetype)
if (NOT ${FREETYPE_FOUND})
  message(FATAL_ERROR "Freetype not found on system! You can use vcpkg (JSON manifest already provided), or provide the dependency another way.")
endif()
target_link_libraries(viszbase PRIVATE ${FREETYPE_LIBRARIES})
target_include_directories(viszbase PUBLIC ${FREETYPE_INCLUDE_DIRS})


	
