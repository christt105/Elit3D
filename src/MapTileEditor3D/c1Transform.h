#pragma once
#include "Component.h"

#include "ExternalTools/MathGeoLib/include/Math/float4x4.h"
#include "ExternalTools/MathGeoLib/include/Math/Quat.h"

class c1Transform : public Component
{
public:
	c1Transform(Object* obj);
	~c1Transform();

	void OnInspector() override;

public:
	void SetMatrix(const float4x4& m);
	float4x4 GetMatrix() const;

	void SetPosition(const float3& pos);
	void SetPosition(float x, float y, float z);

	void SetRotation(const Quat& rot);
	void SetRotation(float x, float y, float z, float w);

	void SetScale(const float3& scl);
	void SetScale(float x, float y, float z);

private:
	float4x4 mat = float4x4::identity;

	float3 position = float3::zero;
	Quat rotation = Quat::identity;
	float3 scale = float3::one;
};

