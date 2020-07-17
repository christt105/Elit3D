#include "c1Mesh.h"
#include <GL/glew.h>

#include "Application.h"
#include "m1Resources.h"

#include "Object.h"

#include "c1Material.h"
#include "c1Transform.h"

#include "r1Mesh.h"
#include "r1Shader.h"

#include "Logger.h"

#include "ExternalTools/ImGui/imgui.h"

#include "ExternalTools/mmgr/mmgr.h"

c1Mesh::c1Mesh(Object* obj) : Component(obj, Type::Mesh), material(obj->CreateComponent<c1Material>())
{
}

c1Mesh::~c1Mesh()
{
}

void c1Mesh::Update()
{
	const r1Mesh* rmesh = ((is_engine_mesh) ? emesh : (r1Mesh*)App->resources->Get(mesh));
	if (rmesh != nullptr) {
		glBindVertexArray(rmesh->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, rmesh->vertices.id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rmesh->indices.id);

		material->shader->SetVec3("color", float3::one);
		material->shader->SetMat4("model", object->transform->GetMatrix());

		material->BindTex();
		glDrawElements(GL_TRIANGLES, rmesh->indices.size, GL_UNSIGNED_INT, (void*)0);
		material->UnBindTex();
	}
	else
		LOGE("Object %s not find mesh %" SDL_PRIu64, object->GetName(), mesh)
}

void c1Mesh::SetMesh(const uint64_t& id)
{
	mesh = id;
	Resource* res = App->resources->Get(mesh);
	res->Attach();
}

void c1Mesh::SetEMesh(m1Resources::EResourceType res)
{
	is_engine_mesh = true;
	emesh = (r1Mesh*)App->resources->Get(res);
	emesh->Attach();
}

void c1Mesh::OnInspector()
{
	if (ImGui::CollapsingHeader("Mesh")) {
		if (mesh != 0U) {
			const r1Mesh* rmesh = (r1Mesh*)App->resources->Get(mesh);
			ImGui::Text("Mesh: "); ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.f, 0.6f, 0.f, 1.f), "%s", rmesh->name.c_str());

			ImGui::Text("UID: "); ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.f, 0.6f, 0.f, 1.f), "%s", std::to_string(rmesh->GetUID()).c_str());

			ImGui::Text("Referneces: "); ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.f, 0.6f, 0.f, 1.f), "%u", rmesh->references);
		}
	}
}
