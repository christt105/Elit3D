#pragma once

#include "ExternalTools/MathGeoLib/include/Math/float3.h"

class Shader;

class Material
{
public:
	Material();
	~Material();

	Shader* shader = nullptr;
private:
	float3 color = float3::one;
};

