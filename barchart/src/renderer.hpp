#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "shader.hpp"
#include "math.hpp"

struct Color
{
  float r, g, b, a;
};

class Renderer
{
public:
  Renderer();

  void drawBox(int x, int y, int x1, int y1, Color color,
      math::Matrix<4, 4>& projection);

private:
  unsigned VAO, VBO;
  Shader rectShader;

  math::Matrix<4, 4> translate;
  math::Matrix<4, 4> scale;
  math::Matrix<4, 4> result;
};

#endif
