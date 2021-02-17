#pragma once

#include <string>
#include "Tools/OpenGL/Buffer.h"
#include "ExternalTools/MathGeoLib/include/Math/float3.h"
#include "ExternalTools/MathGeoLib/include/Math/float2.h"
#include "ExternalTools/MathGeoLib/include/Math/Quat.h"

class r1Shader;

class ObjectEditor
{
	friend class p1ObjectEditor;
public:
	ObjectEditor();
	~ObjectEditor();

	void Draw(r1Shader* shader);
	bool OnGui(bool& sel);

private:
	std::string name = "Mesh";
	char buffer[20]{ "" };

	unsigned int VAO = 0U;
	Buffer<float> vertices;
	Buffer<unsigned int> indices;
	Buffer<float> uv;

	float3 position = float3::zero;
	Quat rot = Quat::identity;
	float3 euler = float3::zero;
	float2 size = float2::one;
	float scale = 1.f;
};

