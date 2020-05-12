#pragma once
#include "Component.h"

//#include "ExternalTools/MathGeoLib/include/Math/float4x4.h"

class c1Transform : public Component
{
public:
	c1Transform(Object* obj);
	~c1Transform();

public:
	//float4x4 mat = float4x4::identity;
};

