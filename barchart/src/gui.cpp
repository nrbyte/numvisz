#include <stdexcept>

#include "glad/gl.h"

#include "gui.hpp"

// Callbacks
static void callback_framebuffer_size(GLFWwindow* window, int width, int height)
{
  GUI* gui = static_cast<GUI*>(glfwGetWindowUserPointer(window));

  gui->width = width;
  gui->height = height;
}

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

  // Correctly set width and height to begin with
  glfwGetFramebufferSize(window, &this->width, &this->height);

  // Allow callbacks to know which GUI object to modify state on upon call
  glfwSetWindowUserPointer(window, this);
  // Register window resize callback
  glfwSetFramebufferSizeCallback(window, callback_framebuffer_size);
}

void GUI::nextFrame()
{
  glfwSwapBuffers(window);
  glfwPollEvents();

  // Update mouse position
  glfwGetCursorPos(window, &this->mouseX, &this->mouseY);
}

GUI::~GUI()
{
  glfwDestroyWindow(window);
  glfwTerminate();
}
