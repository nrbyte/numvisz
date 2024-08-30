#include <stdexcept>

#include <glad/gl.hpp>

#include "renderer.hpp"

Renderer::Renderer()
    : rectShader(
#include "shaders/rect.vs"
          ,
#include "shaders/rect.fs"
      )
{
    // Check the shader compiled successfully
    if (!rectShader.getErrorMsg().empty())
    {
        throw std::runtime_error("Shader error: " + rectShader.getErrorMsg());
    }

    // Configure rectangle vertex buffer
    float vertices[] = {0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,

                        0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f};

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

void Renderer::drawBox(float x, float y, float x1, float y1, Color color,
                       math::Matrix<4, 4>& projection)
{
    glUseProgram(rectShader.getProgram());
    // Setup matrices
    math::setTranslate(translate, x, y, 0.0f);
    math::setScale(scale, (x1 - x), (y1 - y), 1.0f);
    result = projection * translate * scale;
    // Send data to shader
    glUniformMatrix4fv(rectShader.getUniformLocation("matrix"), 1, GL_TRUE,
                       *result);
    glUniform4f(rectShader.getUniformLocation("color"), color.r, color.g,
                color.b, color.a);

    // Draw the rectangle
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
