#include "fontrenderer.hpp"

#include <stdexcept>


#include "glad/gl.hpp"

FontRenderer::FontRenderer()
  : fontShader(
      #include "shaders/font.vs"
      ,
      #include "shaders/font.fs"
    )
{
  // Check the shader compiled successfully
  if (!fontShader.getErrorMsg().empty()) {
    throw std::runtime_error("Shader error: " + fontShader.getErrorMsg());
  }

  // Setup OpenGL rectangle
  float vertices[] = {
    0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
    1.0f, 0.0f, 1.0f,  1.0f, 1.0f,
    0.0f, 1.0f, 1.0f,  0.0f, 0.0f,

    0.0f, 1.0f, 1.0f,  0.0f, 0.0f,
    1.0f, 0.0f, 1.0f,  1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,  1.0f, 0.0f
  };

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float),
      (void*)(3*sizeof(float)));
}

void FontRenderer::loadCharacter(char32_t c)
{
  FT_Load_Char(face, c, FT_LOAD_RENDER);

  unsigned int textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width,
      face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
      face->glyph->bitmap.buffer);

  Character character { 
    textureID, face->glyph->bitmap.width,
      face->glyph->bitmap.rows,
      (unsigned)(face->glyph->advance.x/64),
      face->glyph->bitmap_top, face->glyph->bitmap_left,
      (int)(face->glyph->metrics.horiBearingY/64),
  };

  characterMap[c] = character;
}

void FontRenderer::loadFont(const std::string& filePath, int size)
{
  FT_Error error;

  error = FT_Init_FreeType(&library);
  if (error) {
    throw std::runtime_error("Failed to initialize fonts!");
  }

  error = FT_New_Face(library, filePath.c_str(), 0, &face);
  if (error) {
    throw std::runtime_error("Could not load font file!");
  }

  FT_Set_Char_Size(face, 0, size*64, 96, 96);

  // Allow textures to have a size that isn't a multiple of 4
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // Get overall height of font
  fontHeight = face->size->metrics.height / 64;

  // Load common characters
  for (unsigned char character = 0; character < 255; character++)
  {
    loadCharacter(character);
  }
}

// UTF helper functions
// utf8_charLength - Get amount of bytes a character is
static int utf8_charLength(const char* str)
{
  unsigned char c = str[0];
  if (c < 128) return 1;

  if (((0b11100000) & c) == 0b11000000) return 2;
  if (((0b11110000) & c) == 0b11100000) return 3;
  if (((0b11111000) & c) == 0b11110000) return 4;

  return -1;
}
// Convert UTF8 codepoint into UTF32
static char32_t utf8ToUtf32(const char* c)
{
  char32_t result;

  int length = utf8_charLength(c);

  if (length == 1) result = c[0];
  if (length == 2)
    result = (((0b00011111) & c[0]) << 6) | ((0b00111111) & c[1]);
  if (length == 3)
    result = (((0b00001111) & c[0]) << 12) | (((0b00111111) & c[1]) << 6)
      | ((0b00111111) & c[2]);
  if (length == 4)
    result = (((0b00000111) & c[0]) << 18) | (((0b00111111) & c[1]) << 12)
      | (((0b00111111) & c[2]) << 6) | ((0b00111111) & c[3]);

  return result;
}

void FontRenderer::drawMsg(int x, int y, const std::string& msg,
    math::Matrix<4, 4> projection)
{
  glUseProgram(fontShader.getProgram());

  for (int i = 0; i < msg.length(); )
  {
    char cStart = msg[i];
    char32_t c;
    // Check UTF8 byte length of the character
    if (utf8_charLength(&msg[i]) > 1) {
      // Longer than 1 byte, convert to UTF32 before sending Freetype
      c = utf8ToUtf32(&msg[i]);
    } else { c = cStart; }

    // Check character is loaded in, if not, load it in
    auto it = characterMap.find(c);
    if (it == characterMap.end()) {
      loadCharacter(c);
    }
    Character& ch = characterMap[c];

    // Setup matrices
    math::setTranslate(translate, x + ch.bitmap_left,
        y+(fontHeight-ch.bitmap_top),
        0.0f);
    math::setScale(scale, ch.width, ch.height, 1.0f);
    result = projection * translate * scale;
    // Send data to shader
    glUniformMatrix4fv(fontShader.getUniformLocation("matrix"), 1, GL_TRUE,
        *result);

    // Draw the character
    glBindTexture(GL_TEXTURE_2D, ch.textureID);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    // Advance the x position and move onto the next character
    x += ch.advanceX;
    i += utf8_charLength(&cStart);
  }
}

int FontRenderer::getWidthOfMsg(const std::string& msg)
{
  int width = 0;
  for (int i = 0; i < msg.length(); )
  {
    char cStart = msg[i];
    char32_t c;
    // Check UTF8 byte length of the character
    if (utf8_charLength(&msg[i]) > 1) {
      // Longer than 1 byte, convert to UTF32 before sending Freetype
      c = utf8ToUtf32(&msg[i]);
    } else { c = cStart; }

    // Check character is loaded in, if not, load it in
    auto it = characterMap.find(c);
    if (it == characterMap.end()) {
      loadCharacter(c);
    }

    Character& ch = characterMap[c];
    // And width to total
    width += ch.advanceX;

    i += utf8_charLength(&cStart);
  }

  return width;
}
