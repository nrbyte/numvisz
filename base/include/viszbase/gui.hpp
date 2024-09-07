#ifndef GUI_HPP
#define GUI_HPP

#include <string>

#include <GLFW/glfw3.h>

#include "color.hpp"

class GUI
{
public:
    GUI()
        : width{0}, height{0}, mouseX{0}, mouseY{0}, leftMouseDown{false},
          rightMouseDown{0}
    {
    }

    void setup(int width, int height, const std::string& title);
    ~GUI();

    void nextFrame();

    bool windowStillOpen() { return !glfwWindowShouldClose(window); }

    // GL Helper functions
    void clearScreen(Color color);
    void setViewport(float x, float y, float width, float height);

    // Event state
    int width, height;
    double mouseX, mouseY;
    bool leftMouseDown, rightMouseDown;

private:
    GLFWwindow* window;
};

#endif
