#include "c1Transform.h"

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
