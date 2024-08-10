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
      (int)(face->glyph->metrics.horiBearingY/64)
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
  fontHeight = (face->ascender - face->descender) / 64;
  std::cout << "font height = " << fontHeight << std::endl;

  // Load common characters
  for (unsigned char character = 0; character < 255; character++)
  {
    loadCharacter(character);
  }
}


void FontRenderer::drawMsg(int x, int y, const std::string& msg,
    math::Matrix<4, 4> projection)
{
  glUseProgram(fontShader.getProgram());
  for (char c : msg)
  {
    // Check character is loaded in, if not, load it in
    auto it = characterMap.find(c);
    if (it == characterMap.end()) {
      loadCharacter(c);
    }
    Character& ch = characterMap[c];

    // Setup matrices
    math::setTranslate(translate, x + ch.bitmap_left,
        (y+(fontHeight - ch.bearingY)),
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

    x += ch.advanceX;
  }
}
