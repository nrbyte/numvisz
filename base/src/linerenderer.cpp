#include "viszbase/linerenderer.hpp"

#include <glad/gl.hpp>

// Renderer
LineRenderer::LineRenderer(const std::vector<float>& points)
    : lineShader(
#include "shaders/line.vs"
          ,
#include "shaders/line.gs"
          ,
#include "shaders/line.fs"
      )
{
    // Check the shader compiled successfully
    if (!lineShader.getErrorMsg().empty())
    {
        throw std::runtime_error("Line shader error: " +
                                 lineShader.getErrorMsg());
    }

    numOfPoints = points.size();

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, numOfPoints * sizeof(float), points.data(),
                 GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindVertexArray(0);
}
void LineRenderer::draw(Color color, float aspectRatio, float lineThickness,
                        const math::Matrix<4, 4>& proj)
{
    glBindVertexArray(VAO);
    glUseProgram(lineShader.getProgram());

    // Send shader values
    glUniformMatrix4fv(lineShader.getUniformLocation("matrix"), 1, GL_TRUE,
                       *proj);
    glUniform4f(lineShader.getUniformLocation("color"), color.r, color.g,
                color.b, color.a);
    glUniform1f(lineShader.getUniformLocation("aspectRatio"), aspectRatio);
    glUniform1f(lineShader.getUniformLocation("lineThickness"), lineThickness);

    // Draw
    glDrawArrays(GL_LINE_STRIP, 0, numOfPoints / 2);
}

// Builder
void LineRendererBuilder::addPoint(float x, float y)
{
    points.push_back(x);
    points.push_back(y);
}

LineRenderer LineRendererBuilder::build() { return LineRenderer(points); }
