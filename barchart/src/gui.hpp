#ifndef GUI_HPP
#define GUI_HPP

#include <string>

#include <GLFW/glfw3.h>

class GUI 
{
public:
  void setup(int width, int height, const std::string& title);
  ~GUI();

  void nextFrame();
  
  bool windowStillOpen() {
    return !glfwWindowShouldClose(window);
  }

  // Event state
  int width, height;
  double mouseX, mouseY;

private:
  GLFWwindow* window;
};

#endif
