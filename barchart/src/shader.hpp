#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <unordered_map>

class Shader
{
public:
  Shader(const char* vsSource, const char* fsSource);
  unsigned getUniformLocation(const std::string&);

  unsigned getProgram() {
    return program;
  }

  std::string& getErrorMsg() {
    return errorMsg;
  }
private:
  unsigned program;
  std::unordered_map<std::string, unsigned int> uniformLocations;

  std::string errorMsg;
};

#endif
