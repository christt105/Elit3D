#include "Tools/OpenGL/ObjectEditor.h"
#include "Tools/OpenGL/OpenGLHelper.h"

#include "Core/Application.h"
#include "Modules/m1Render3D.h"
#include "Resources/r1Shader.h"

#include "ExternalTools/ImGui/imgui.h"
#include "ExternalTools/MathGeoLib/include/Math/float4x4.h"

ObjectEditor::ObjectEditor() {
	vertices.size = 4; //Quad
	vertices.data = new float[vertices.size * 3]{
		0.f, 0.f, 0.f,
		1.f, 0.f, 0.f,
		1.f, 1.f, 0.f,
		0.f, 1.f, 0.f
	};
	indices.size = 6;
	indices.data = new unsigned int[indices.size]{
		0, 2, 1,
		0, 3, 2
	};
	uv.size = vertices.size;
	uv.data = new float[uv.size * 2]{
		0.f, 0.f,
		1.f, 0.f,
		1.f, 1.f,
		0.f, 1.f
	};

	oglh::GenVAO(VAO);
	oglh::GenArrayBuffer(vertices.id, vertices.size, sizeof(float), 3, vertices.data, 0, 3);
	oglh::GenElementBuffer(indices.id, indices.size, indices.data);
	oglh::GenArrayBuffer(uv.id, uv.size, sizeof(float), 2, uv.data, 1, 2);
}

ObjectEditor::~ObjectEditor() {
	oglh::DeleteVAO(VAO, vertices.id, indices.id);
	oglh::DeleteBuffer(uv.id);

	delete[] vertices.data;
	delete[] indices.data;
	delete[] uv.data;
}

void ObjectEditor::Draw(r1Shader* shader)
{
	oglh::BindBuffers(VAO, vertices.id, indices.id);

	shader->SetMat4("model", float4x4::FromTRS(position, rot, float3(size, 1.f)));

	oglh::DrawElements(indices.size);
}

bool ObjectEditor::OnGui()
{
	bool ret = true;
	ImGui::PushID(this);
	if (ImGui::TreeNodeEx(this, ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_NoTreePushOnOpen, name.c_str())) {
		ImGui::Indent();

		static char buf[20];
		strcpy(buf, name.c_str());
		ImGui::InputText("Name", buf, 20);
		name.assign(buf);
		ImGui::DragFloat3("Position", position.ptr(), 0.01f);
		if (ImGui::DragFloat3("Rotation", euler.ptr(), 1.f, -360.f, 360.f))	rot = Quat::FromEulerXYZ(math::DegToRad(euler.x), math::DegToRad(euler.y), math::DegToRad(euler.z));
		ImGui::DragFloat2("Size", size.ptr(), 0.1f);

		ImGui::PushStyleColor(ImGuiCol_Button, { 1.f, 0.f, 0.f, 1.f });
		ret = !ImGui::Button("Delete");
		ImGui::PopStyleColor();
		
		ImGui::Unindent();
	}
	ImGui::PopID();
	return ret;
}
