#include <stdexcept>

#define GLAD_GL_IMPLEMENTATION
#include "glad/gl.h"

#include "application.hpp"
#include "shader.hpp"

int Application::run()
{
  gui.setup(800, 600, "Visualization");

  // Draw a temporary triangle for now
  Shader shader(
      #include "shaders/rect.vs"
      ,
      #include "shaders/rect.fs"
  );
  if (!shader.getErrorMsg().empty()) {
    throw std::runtime_error("Shader error: " + shader.getErrorMsg());
  }

  float vertices[] = {
    -0.5f, -0.5f, 1.0f,
     0.0f,  0.5f, 1.0f,
     0.5f, -0.5f, 1.0f
  };

  unsigned VAO, VBO;

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

  while (gui.windowStillOpen())
  {
    // Clear the frame
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    // Set viewport size
    glViewport(0, 0, gui.width, gui.height);

    // Draw a triangle for now
    glUseProgram(shader.getProgram());
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Tell GUI system we're finished rendering
    gui.nextFrame();
  }
  return 0;
}
