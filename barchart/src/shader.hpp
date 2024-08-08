#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>

class Shader
{
public:
  Shader(const char* vsSource, const char* fsSource);

  unsigned getProgram() {
    return program;
  }

  std::string& getErrorMsg() {
    return errorMsg;
  }
private:
  unsigned program;

  std::string errorMsg;
};

#endif
