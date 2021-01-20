#include "Objects/Components/c1Transform.h"

#include "ExternalTools/MathGeoLib/include/MathGeoLib.h"

#include "Objects/Object.h"

#include "ExternalTools/ImGui/imgui.h"

c1Transform::c1Transform(Object* obj) : Component(obj, Type::Transform)
{
	CalculateGlobalMatrix();
}

c1Transform::~c1Transform()
{
}

void c1Transform::OnInspector()
{
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
		bool changed = false;
		if (ImGui::DragFloat3("Position", position.ptr()))
			changed = true;
		float3 euler = rotation.ToEulerXYZ();
		if (ImGui::DragFloat3("Rotation", euler.ptr())) {
			changed = true;
			rotation = Quat::FromEulerXYZ(euler.x, euler.y, euler.z);
		}
		if(ImGui::DragFloat3("Scale", scale.ptr()))
			changed = true;

		if (changed)
			CalculateGlobalMatrix();
	}
}

void c1Transform::SetLocalMatrix(const float4x4& m)
{
	m.Decompose(position, rotation, scale);

	CalculateGlobalMatrix();
}

const float4x4& c1Transform::GetLocalMatrix() const
{
	return mat;
}

const float4x4& c1Transform::GetGlobalMatrix() const
{
	return gmat;
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

void c1Transform::CalculateGlobalMatrix()
{
	mat = gmat = float4x4::FromTRS(position, rotation, scale);
	if(object->parent != nullptr) {
		gmat = object->parent->transform->GetGlobalMatrix() * mat;
	}

	for (auto i = object->children.begin(); i != object->children.end(); ++i) {
		(*i)->transform->CalculateGlobalMatrix();
	}
}
