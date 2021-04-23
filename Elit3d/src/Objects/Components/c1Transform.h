#pragma once
#include "Objects/Components/Base/Component.h"

#include "ExternalTools/MathGeoLib/include/Math/float4x4.h"
#include "ExternalTools/MathGeoLib/include/Math/Quat.h"

namespace pugi {
	class xml_node;
}

class c1Transform : public Component
{
public:
	c1Transform(Object* obj);
	~c1Transform();

	void OnInspector() override;

public:
	void SetLocalMatrix(const float4x4& m);
	const float4x4& GetLocalMatrix() const;
	const float4x4& GetGlobalMatrix() const;

	void SetPosition(const float3& pos);
	void SetPosition(float x, float y, float z);

	void SetRotation(const Quat& rot);
	void SetRotation(float x, float y, float z, float w);

	void SetScale(const float3& scl);
	void SetScale(float x, float y, float z);

	void CalculateGlobalMatrix();

	nlohmann::json	Parse() override;
	void			Unparse(const nlohmann::json& node) override;

	void Serialize(nlohmann::json& node) const;
	void Serialize(pugi::xml_node& node) const;

private:
	float4x4 mat = float4x4::identity;
	float4x4 gmat = float4x4::identity;

	float3 position = float3::zero;
	Quat rotation = Quat::identity;
	float3 euler = float3::zero;
	float3 scale = float3::one;
};

