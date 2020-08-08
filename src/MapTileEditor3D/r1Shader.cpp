#include "r1Shader.h"
#include <GL/glew.h>
#include "Application.h"
#include "FileSystem.h"
#include <string>
#include "Logger.h"

#include "ExternalTools/mmgr/mmgr.h"

r1Shader::r1Shader(const char* vertexPath, const char* fragmentPath)
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

    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexCode, NULL);
    glCompileShader(vertex);
    CheckCompileErrors(vertex, Type::VERTEX);

    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentCode, NULL);
    glCompileShader(fragment);
    CheckCompileErrors(fragment, Type::FRAGMENT);

    id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    glLinkProgram(id);
    CheckCompileErrors(id, Type::PROGRAM);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

r1Shader::~r1Shader()
{
}

void r1Shader::Use()
{
    glUseProgram(id);
}

unsigned int r1Shader::GetID() const
{
    return id;
}

void r1Shader::SetBool(const char* name, bool value)
{
    auto it = uniform_cache.find(name);
    if (it != uniform_cache.end())
        glUniform1i((*it).second, value);
    else {
        int loc = glGetUniformLocation(id, name);
        if (loc != -1) {
            uniform_cache[name] = loc;
            glUniform1i(loc, value);
        }
        else
            LOGW("Variable %s not found in %i shader", name, id);
    }
}

void r1Shader::SetInt(const char* name, int value)
{
    auto it = uniform_cache.find(name);
    if (it != uniform_cache.end())
        glUniform1i((*it).second, value);
    else {
        int loc = glGetUniformLocation(id, name);
        if (loc != -1) {
            uniform_cache[name] = loc;
            glUniform1i(loc, value);
        }
        else
            LOGW("Variable %s not found in %i shader", name, id);
    }
}

void r1Shader::SetFloat(const char* name, float value)
{
    auto it = uniform_cache.find(name);
    if (it != uniform_cache.end())
        glUniform1f((*it).second, value);
    else {
        int loc = glGetUniformLocation(id, name);
        if (loc != -1) {
            uniform_cache[name] = loc;
            glUniform1f(loc, value);
        }
        else
            LOGW("Variable %s not found in %i shader", name, id);
    }
}

void r1Shader::SetVec3(const char* name, const float3& value)
{
    auto it = uniform_cache.find(name);
    if(it != uniform_cache.end())
        glUniform3fv((*it).second, 1, value.ptr());
    else {
        int loc = glGetUniformLocation(id, name);
        if (loc != -1) {
            uniform_cache[name] = loc;
            glUniform3fv(loc, 1, value.ptr());
        }
        else
            LOGW("Variable %s not found in %i shader", name, id);
    }
}

void r1Shader::SetMat4(const char* name, const float4x4& value)
{
    auto it = uniform_cache.find(name);
    if (it != uniform_cache.end())
        glUniformMatrix4fv((*it).second, 1, GL_TRUE, value.ptr());
    else {
        int loc = glGetUniformLocation(id, name);
        if (loc != -1) {
            uniform_cache[name] = loc;
            glUniformMatrix4fv(loc, 1, GL_TRUE, value.ptr());
        }
        else
            LOGW("Variable %s not found in %i shader", name, id);
    }
}

void r1Shader::CheckCompileErrors(unsigned int shader, Type type)
{
    int success = 0;
    static char infoLog[1024];

    if (type == Type::PROGRAM) {
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
            if (type == Type::VERTEX) {
                LOGE("SHADER::VERTEX::COMPILATION_FAILED : %s", infoLog);
            }
            else
                LOGE("SHADER::FRAGMENT::COMPILATION_FAILED : %s", infoLog);
        }
    }
}
