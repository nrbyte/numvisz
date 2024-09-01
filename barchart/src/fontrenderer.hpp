#ifndef FONTRENDERER_HPP
#define FONTRENDERER_HPP

#include <string>
#include <unordered_map>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "math.hpp"
#include "shader.hpp"
#include "commandlineparser.hpp"

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

    void drawMsg(float x, float y, const std::string& msg,
                 math::Matrix<4, 4> projection);
    void drawLongDouble(float x, float y, const long double& num,
                        int decimalPoints, math::Matrix<4, 4> projection);

    int getWidthOfMsg(const std::string& msg);
    int getWidthOfLongDouble(const long double& num, int decimalPoints);

    int getFontHeight() const { return fontHeight; }

private:
    FT_Library library;
    FT_Face face;
    struct Character
    {
        unsigned textureID;
        unsigned width, height;
        unsigned advanceX;

        int bitmap_top, bitmap_left;
        int bearingY;
    };

    int fontHeight;
    int yMax;
    int yMin;

    unsigned VAO, VBO;
    Shader fontShader;

    std::unordered_map<char32_t, Character> characterMap;

    math::Matrix<4, 4> translate;
    math::Matrix<4, 4> scale;
    math::Matrix<4, 4> result;

    void loadCharacter(char32_t c);
};
#endif
