#include <iostream>
#include <stdexcept>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

#include "application.hpp"

int Application::run()
{
  // Initialize
  try
  {
    gui.setup(800, 600, "Visualization");
  }
  catch(std::runtime_error e)
  {
    // Prefix all errors with 'ERROR:'
    std::cerr << "ERROR:" << e.what() << std::endl;
    return -1;
  }

  while (gui.windowStillOpen())
  {
    // Clear the frame
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Tell GUI system we're finished rendering
    gui.nextFrame();
  }
  return 0;
}
