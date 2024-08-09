#ifndef FONTRENDERER_HPP
#define FONTRENDERER_HPP

#include <string>
#include <unordered_map>

#include "math.hpp"
#include "shader.hpp"

#ifndef STRUCT_COLOR
#define STRUCT_COLOR
struct Color
{
  float r, g, b, a;
};
#endif

class FontRenderer
{
public:
  FontRenderer();
  void loadFont(const std::string& filePath, int size);

  void drawMsg(int x, int y, const std::string& msg,
      math::Matrix<4, 4> projection);

private:
  unsigned VAO, VBO;
  Shader fontShader;

  std::unordered_map<char32_t, unsigned> characterMap;

  math::Matrix<4, 4> translate;
  math::Matrix<4, 4> scale;
  math::Matrix<4, 4> result;
};
#endif
