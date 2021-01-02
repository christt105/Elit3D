#pragma once

#include <unordered_map>

#include "ExternalTools/MathGeoLib/include/Math/float4x4.h"
#include "ExternalTools/MathGeoLib/include/Math/float2.h"

class int2;

class r1Shader
{
private:
	friend class m1Render3D;
	enum class Type {
		PROGRAM, VERTEX, FRAGMENT
	};
public:
	r1Shader();
	~r1Shader();

	void Use();
	unsigned int GetID() const;

	void SetBool (const char* name, bool value);
	void SetInt  (const char* name, int value);
	void SetFloat(const char* name, float value);
	void SetInt2 (const char* name, const int2& value);
	void SetVec2 (const char* name, const float2& value);
	void SetVec3 (const char* name, const float3& value);
	void SetMat4 (const char* name, const float4x4& value);

	void Link(unsigned int vertex, unsigned int fragment);

protected:
	unsigned int id;

	std::string identifier;

	void SetName(const char* nname);

private:
	static unsigned int Compile(const std::string& filepath);
	static void CheckCompileErrors(unsigned int shader, Type type);

	std::unordered_map<std::string, int> uniform_cache;
};

