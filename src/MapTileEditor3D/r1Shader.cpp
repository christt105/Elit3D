#include "r1Shader.h"
#include <GL/glew.h>
#include <string>
#include "Application.h"
#include "FileSystem.h"
#include "int2.h"
#include "Logger.h"

#include "ExternalTools/mmgr/mmgr.h"

r1Shader::r1Shader()
{
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
            LOGW("Variable %s not found in %s shader", name, identifier.c_str());
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
            LOGW("Variable %s not found in %s shader", name, identifier.c_str());
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
            LOGW("Variable %s not found in %s shader", name, identifier.c_str());
    }
}

void r1Shader::SetInt2(const char* name, const int2& value)
{
    auto it = uniform_cache.find(name);
    if (it != uniform_cache.end())
        glUniform2iv((*it).second, 1, value.ptr());
    else {
        int loc = glGetUniformLocation(id, name);
        if (loc != -1) {
            uniform_cache[name] = loc;
            glUniform2iv(loc, 1, value.ptr());
        }
        else
            LOGW("Variable %s not found in %s shader", name, identifier.c_str());
    }
}

void r1Shader::SetVec2(const char* name, const float2& value)
{
    auto it = uniform_cache.find(name);
    if (it != uniform_cache.end())
        glUniform2fv((*it).second, 1, value.ptr());
    else {
        int loc = glGetUniformLocation(id, name);
        if (loc != -1) {
            uniform_cache[name] = loc;
            glUniform3fv(loc, 1, value.ptr());
        }
        else
            LOGW("Variable %s not found in %s shader", name, identifier.c_str());
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
            LOGW("Variable %s not found in %s shader", name, identifier.c_str());
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
            LOGW("Variable %s not found in %s shader", name, identifier.c_str());
    }
}

void r1Shader::Link(unsigned int vertex, unsigned int fragment)
{
    id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    glLinkProgram(id);
    CheckCompileErrors(id, Type::PROGRAM);
}

void r1Shader::SetName(const char* nname)
{
    identifier.assign(nname);
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

unsigned int r1Shader::Compile(const std::string& filepath)
{
    std::string shaderSource;
    try {
        shaderSource = FileSystem::OpenTextFile(filepath.c_str());
    }
    catch (const char* fail) {
        LOGE("FAILED TO READ SHADER %s", fail);
    }

    const char* code = shaderSource.c_str();
    unsigned int ret = 0;

    if (filepath.find(".vertex.glsl") != std::string::npos) {
        ret = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(ret, 1, &code, NULL);
        glCompileShader(ret);
        CheckCompileErrors(ret, Type::VERTEX);
    }
    else if (filepath.find(".fragment.glsl") != std::string::npos) {
        ret = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(ret, 1, &code, NULL);
        glCompileShader(ret);
        CheckCompileErrors(ret, Type::FRAGMENT);
    }
    else {
        LOG("Syntaxi of shader %s not allowed", filepath.c_str());
    }
    
    return ret;
}
