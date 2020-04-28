#pragma once

#include "ExternalTools/glm/glm/glm.hpp"

class Shader
{
private:
	enum Type {
		PROGRAM, VERTEX, FRAGMENT
	};
public:
	Shader(const char* vertexPath, const char* fragmentPath);
	~Shader();

	void Use();

	void SetBool(const char* name, bool value) const;
	void SetInt(const char* name, int value) const;
	void SetFloat(const char* name, float value) const;
	void SetMat4(const char* name, const glm::mat4& value) const;

protected:
	unsigned int id;

private:
	void CheckCompileErrors(unsigned int shader, Type type);
};

