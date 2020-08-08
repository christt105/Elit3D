#pragma once

#include <unordered_map>

#include "ExternalTools/MathGeoLib/include/Math/float4x4.h"

class r1Shader
{
private:
	enum class Type {
		PROGRAM, VERTEX, FRAGMENT
	};
public:
	r1Shader(const char* vertexPath, const char* fragmentPath);
	~r1Shader();

	void Use();
	unsigned int GetID() const;

	void SetBool(const char* name, bool value);
	void SetInt(const char* name, int value);
	void SetFloat(const char* name, float value);
	void SetVec3(const char* name, const float3& value);
	void SetMat4(const char* name, const float4x4& value);

protected:
	unsigned int id;

private:
	void CheckCompileErrors(unsigned int shader, Type type);

	std::unordered_map<std::string, int> uniform_cache;
};

