#include "Objects/Components/c1Transform.h"

#include "ExternalTools/MathGeoLib/include/MathGeoLib.h"

#include "ExternalTools/ImGui/imgui.h"

c1Transform::c1Transform(Object* obj) : Component(obj, Type::Transform)
{
}

c1Transform::~c1Transform()
{
}

void c1Transform::OnInspector()
{
	if (ImGui::CollapsingHeader("Transform")) {
		float3 pos, euler, scale;
		Quat rot;
		mat.Decompose(pos, rot, scale);
		euler = rot.ToEulerXYZ();
		ImGui::DragFloat3("Position", pos.ptr());
		ImGui::DragFloat3("Rotation", euler.ptr());
		ImGui::DragFloat3("Scale", scale.ptr());
	}
}

void c1Transform::SetMatrix(const float4x4& m)
{
	mat = m;
}

float4x4 c1Transform::GetMatrix() const
{
	return mat;
}

void c1Transform::SetPosition(const float3& pos)
{
	position = pos;

	mat = float4x4::FromTRS(position, rotation, scale);
}

void c1Transform::SetPosition(float x, float y, float z)
{
	position = float3(x, y, z);

	mat = float4x4::FromTRS(position, rotation, scale);
}

void c1Transform::SetRotation(const Quat& rot)
{
	rotation = rot;

	mat = float4x4::FromTRS(position, rotation, scale);
}

void c1Transform::SetRotation(float x, float y, float z, float w)
{
	rotation = Quat(x, y, z, w);

	mat = float4x4::FromTRS(position, rotation, scale);
}

void c1Transform::SetScale(const float3& scl)
{
	scale = scl;

	mat = float4x4::FromTRS(position, rotation, scale);
}

void c1Transform::SetScale(float x, float y, float z)
{
	scale = float3(x, y, z);

	mat = float4x4::FromTRS(position, rotation, scale);
}
