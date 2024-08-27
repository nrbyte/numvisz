#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "shader.hpp"
#include "math.hpp"

#ifndef STRUCT_COLOR
#define STRUCT_COLOR
struct Color
{
  float r, g, b, a;
};
#endif

class Renderer
{
public:
  Renderer();

  void drawBox(float x, float y, float x1, float y1, Color color,
      math::Matrix<4, 4>& projection);

private:
  unsigned VAO, VBO;
  Shader rectShader;

  math::Matrix<4, 4> translate;
  math::Matrix<4, 4> scale;
  math::Matrix<4, 4> result;
};
#endif
