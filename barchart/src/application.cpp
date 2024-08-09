
#define GLAD_GL_IMPLEMENTATION
#include "glad/gl.hpp"

#include "application.hpp"
#include "renderer.hpp"
#include "math.hpp"

int Application::run()
{
  gui.setup(800, 600, "Visualization");

  math::Matrix<4, 4> proj;
  Renderer renderer;

  while (gui.windowStillOpen())
  {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    // Set viewport size
    glViewport(0, 0, gui.width, gui.height);

    // Update projection matrix
    math::setOrtho(proj, 0, gui.width, gui.height, 0, -0.1f, -100.0f);

    renderer.drawBox(10, 10, gui.width-10, gui.height-10, Color {0.4f, 0.9f, 0.8f, 1.0f}, proj);

    gui.nextFrame();
  }
  return 0;
}
