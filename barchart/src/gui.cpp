#include <stdexcept>

#include <glad/gl.h>

#include "gui.hpp"

void GUI::setup(int width, int height, const std::string& title)
{
  if (glfwInit() == GLFW_FALSE) {
    throw std::runtime_error("Failed to initialize GUI");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
  if (!window) {
    throw std::runtime_error("Failed to create window!");
  }

  glfwMakeContextCurrent(window);
  gladLoadGL(glfwGetProcAddress);

  glfwSwapInterval(1);
}

void GUI::nextFrame()
{
  glfwSwapBuffers(window);
  glfwPollEvents();
}

GUI::~GUI()
{
  glfwDestroyWindow(window);
  glfwTerminate();
}
