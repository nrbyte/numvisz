#include "glad/gl.h"

#include "shader.hpp"

Shader::Shader(const char* vsSource, const char* fsSource)
{
  // Compile the vertex shader
  unsigned vShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vShader, 1, &vsSource, NULL);
  glCompileShader(vShader);

  int status = 0;
  char msg[1024];

  glGetShaderiv(vShader, GL_COMPILE_STATUS, &status);
  if (!status)
  {
    errorMsg += "\nVertex Shader failed to compile:\n";
    glGetShaderInfoLog(vShader, 1024, NULL, msg);
    errorMsg += msg;
    errorMsg += '\n';
  }

  // Compile the fragment shader
  unsigned fShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fShader, 1, &fsSource, NULL);
  glCompileShader(fShader);

  status = 0;

  glGetShaderiv(fShader, GL_COMPILE_STATUS, &status);
  if (!status)
  {
    errorMsg += "\nFragment Shader failed to compile:\n";
    glGetShaderInfoLog(fShader, 1024, NULL, msg);
    errorMsg += msg;
    errorMsg += '\n';
  }

  // Link the program
  program = glCreateProgram();
  glAttachShader(program, vShader);
  glAttachShader(program, fShader);
  glLinkProgram(program);

  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (!status)
  {
    glGetProgramInfoLog(program, 1024, NULL, msg);
    errorMsg += "\nProgram failed to link:\n";
    errorMsg += msg;
    errorMsg += '\n';
  }

  glDeleteShader(vShader);
  glDeleteShader(fShader);
}

unsigned Shader::getUniformLocation(const std::string& name)
{
  // Look for the corresponding uniform location
  auto it = uniformLocations.find(name);
  // If found, return it
  if (it != uniformLocations.end()) {
    return uniformLocations[name];
  }
  // Otherwise, get the uniform location, add it to the map, and then return it
  uniformLocations[name] = glGetUniformLocation(program, name.c_str());
  return uniformLocations[name];
}
