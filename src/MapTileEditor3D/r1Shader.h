#pragma once

#include "ExternalTools/MathGeoLib/include/Math/float4x4.h"

class r1Shader
{
private:
	enum Type {
		PROGRAM, VERTEX, FRAGMENT
	};
public:
	r1Shader(const char* vertexPath, const char* fragmentPath);
	~r1Shader();

	void Use();
	unsigned int GetID() const;

	void SetBool(const char* name, bool value) const;
	void SetInt(const char* name, int value) const;
	void SetFloat(const char* name, float value) const;
	void SetVec3(const char* name, const float3& value) const;
	void SetMat4(const char* name, const float4x4& value) const;

protected:
	unsigned int id;

private:
	void CheckCompileErrors(unsigned int shader, Type type);
};

