#ifndef LINE_RENDERER_HPP
#define LINE_RENDERER_HPP

#include "viszbase/color.hpp"
#include "viszbase/math.hpp"
#include "viszbase/shader.hpp"

#include <vector>

class LineRendererBuilder;

class LineRenderer
{
public:
    void draw(Color color, float aspectRatio, float lineThickness,
              const math::Matrix<4, 4>& proj);

private:
    LineRenderer(std::vector<float>& points);

    unsigned VAO, VBO, numOfPoints;
    Shader lineShader;

    friend class LineRendererBuilder;
};

class LineRendererBuilder
{
public:
    void addPoint(float x, float y);
    LineRenderer build();

private:
    std::vector<float> points;
};

#endif
