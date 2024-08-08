#include <algorithm>
#include <stdexcept>

#define GLAD_GL_IMPLEMENTATION
#include "glad/gl.h"

#include "application.hpp"
#include "shader.hpp"
#include "math.hpp"

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

  math::Matrix<4, 4> proj;
  math::Matrix<4, 4> scale;
  math::Matrix<4, 4> translate;
  
  math::setOrtho(proj, 0, 800, 600, 0, -0.1f, -100.0f);
  math::setScale(scale, 100, 100, 1);
  math::setTranslate(translate, 100.0f, 100.0f, 0.0f);

  math::Matrix<4, 4> matrix = proj * (translate * scale);

  while (gui.windowStillOpen())
  {
    // Clear the frame
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    // Set viewport size
    glViewport(0, 0, gui.width, gui.height);

    // Draw a triangle for now
    glUseProgram(shader.getProgram());
    glUniformMatrix4fv(shader.getUniformLocation("matrix"),
        1, GL_TRUE, *matrix);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Tell GUI system we're finished rendering
    gui.nextFrame();
  }
  return 0;
}
