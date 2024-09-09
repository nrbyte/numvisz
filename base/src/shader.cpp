#include "glad/gl.hpp"

#include "viszbase/shader.hpp"

unsigned Shader::createShader(unsigned shaderType, const char* source)
{
    // Compile the shader
    unsigned shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int status = 0;
    char msg[1024];

    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status)
    {
        switch (shaderType)
        {
        case GL_VERTEX_SHADER:
            errorMsg += "\nVertex Shader failed to compile:\n";
            break;
        case GL_GEOMETRY_SHADER:
            errorMsg += "\nGeometry Shader failed to compile:\n";
            break;
        case GL_FRAGMENT_SHADER:
            errorMsg += "\nFragment Shader failed to compile:\n";
            break;
        default:
            errorMsg += "\nUnrecognised shader failed to compile:\n";
        }
        glGetShaderInfoLog(shader, 1024, NULL, msg);
        errorMsg += msg;
        errorMsg += '\n';
    }

    return shader;
}

Shader::Shader(const char* vsSource, const char* fsSource)
{
    // Compile the vertex shader
    unsigned vShader = createShader(GL_VERTEX_SHADER, vsSource);

    // Compile the fragment shader
    unsigned fShader = createShader(GL_FRAGMENT_SHADER, fsSource);

    // Link the program
    int status;
    char msg[1024];

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

Shader::Shader(const char* vsSource, const char* gsSource, const char* fsSource)
{
    // Compile the vertex shader
    unsigned vShader = createShader(GL_VERTEX_SHADER, vsSource);

    // Compile the geometry shader
    unsigned gShader = createShader(GL_GEOMETRY_SHADER, gsSource);

    // Compile the fragment shader
    unsigned fShader = createShader(GL_FRAGMENT_SHADER, fsSource);

    // Link the program
    int status;
    char msg[1024];

    program = glCreateProgram();
    glAttachShader(program, vShader);
    glAttachShader(program, gShader);
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
    glDeleteShader(gShader);
    glDeleteShader(fShader);
}

unsigned Shader::getUniformLocation(const std::string& name)
{
    // Look for the corresponding uniform location
    auto it = uniformLocations.find(name);
    // If found, return it
    if (it != uniformLocations.end())
    {
        return uniformLocations[name];
    }
    // Otherwise, get the uniform location, add it to the map, and then return
    // it
    uniformLocations[name] = glGetUniformLocation(program, name.c_str());
    return uniformLocations[name];
}
