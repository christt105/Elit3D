#include "Shader.h"
#include <GL/glew.h>
#include "Application.h"
#include "FileSystem.h"
#include <string>
#include "Logger.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    std::string vertexShaderSource;
    std::string fragmentShaderSource;
    try {
        vertexShaderSource = App->file_system->OpenTextFile(vertexPath);
        fragmentShaderSource = App->file_system->OpenTextFile(fragmentPath);
    }
    catch (const char* fail) {
        LOGE("FAILED TO READ SHADER %s", fail);
    }

    const char* vertexCode = vertexShaderSource.c_str();
    const char* fragmentCode = fragmentShaderSource.c_str();
    unsigned int vertex, fragment;

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexCode, NULL);
    glCompileShader(vertex);
    CheckCompileErrors(vertex, VERTEX);

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentCode, NULL);
    glCompileShader(fragment);
    CheckCompileErrors(fragment, FRAGMENT);

    id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    glLinkProgram(id);
    CheckCompileErrors(id, PROGRAM);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::~Shader()
{
}

void Shader::Use()
{
    glUseProgram(id);
}

void Shader::CheckCompileErrors(unsigned int shader, Type type)
{
    int success = 0;
    static char infoLog[1024];

    if (type == PROGRAM) {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            LOGE("SHADER::PROGRAM::LINKING_FAILED : %s", infoLog);
        }
    }
    else {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            if (type == VERTEX) {
                LOGE("SHADER::VERTEX::COMPILATION_FAILED : %s", infoLog);
            }
            else
                LOGE("SHADER::FRAGMENT::COMPILATION_FAILED : %s", infoLog);
        }
    }
}
