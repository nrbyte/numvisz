#include <stdexcept>

#define GLAD_GLAPI_EXPORT
#define GLAD_GLAPI_EXPORT_BUILD
#define GLAD_GL_IMPLEMENTATION
#include "glad/gl.hpp"

#include "viszbase/gui.hpp"

// Callbacks
static void callback_framebuffer_size(GLFWwindow* window, int width, int height)
{
    GUI* gui = static_cast<GUI*>(glfwGetWindowUserPointer(window));

    gui->width = width;
    gui->height = height;
}
static void callback_mouse_button(GLFWwindow* window, int button, int action,
                                  int mods)
{
    if (action == GLFW_PRESS)
    {
        GUI* gui = static_cast<GUI*>(glfwGetWindowUserPointer(window));
        if (button == GLFW_MOUSE_BUTTON_LEFT)
            gui->leftMouseDown = true;
        if (button == GLFW_MOUSE_BUTTON_RIGHT)
            gui->rightMouseDown = true;
    }
    else if (action == GLFW_RELEASE)
    {
        GUI* gui = static_cast<GUI*>(glfwGetWindowUserPointer(window));
        if (button == GLFW_MOUSE_BUTTON_LEFT)
            gui->leftMouseDown = false;
        if (button == GLFW_MOUSE_BUTTON_RIGHT)
            gui->rightMouseDown = false;
    }
}

void GUI::setup(int width, int height, const std::string& title)
{
    if (glfwInit() == GLFW_FALSE)
    {
        throw std::runtime_error("Failed to initialize GUI");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    if (!window)
    {
        throw std::runtime_error("Failed to create window!");
    }

    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);

    glEnable(GL_MULTISAMPLE);

    glfwSwapInterval(1);

    // Correctly set width and height to begin with
    glfwGetFramebufferSize(window, &this->width, &this->height);

    // Allow callbacks to know which GUI object to modify state on upon call
    glfwSetWindowUserPointer(window, this);
    // Register window resize callback
    glfwSetFramebufferSizeCallback(window, callback_framebuffer_size);
    // Register mouse input callback
    glfwSetMouseButtonCallback(window, callback_mouse_button);
}

void GUI::nextFrame()
{
    glfwSwapBuffers(window);
    glfwPollEvents();

    // Update mouse position
    glfwGetCursorPos(window, &this->mouseX, &this->mouseY);
}

void GUI::clearScreen(Color color)
{
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void GUI::setViewport(float x, float y, float width, float height)
{
    glViewport(x, y, width, height);
}

GUI::~GUI()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}
