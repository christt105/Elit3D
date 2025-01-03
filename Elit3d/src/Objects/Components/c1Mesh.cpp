#include "Objects/Components/c1Mesh.h"

#include "Core/Application.h"
#include "Modules/m1Resources.h"

#include "Objects/Object.h"

#include "Objects/Components/c1Material.h"
#include "Objects/Components/c1Transform.h"

#include "Resources/r1Model.h"
#include "Resources/r1Mesh.h"
#include "Resources/r1Shader.h"

#include "Tools/System/Logger.h"

#include "Tools/OpenGL/OpenGLHelper.h"

#include "ExternalTools/ImGui/imgui.h"

#include "ExternalTools/mmgr/mmgr.h"

c1Mesh::c1Mesh(Object* obj) : Component(obj, Type::Mesh)
{
	material = obj->GetComponent<c1Material>();
	if (material == nullptr)
		material = obj->CreateComponent<c1Material>();
}

c1Mesh::~c1Mesh()
{
}

void c1Mesh::Update()
{
	const r1Mesh* rmesh = ((is_engine_mesh) ? emesh : (r1Mesh*)App->resources->Get(mesh));
	if (rmesh != nullptr) {
		material->shader->Use();

		oglh::ActiveTexture(0);

		oglh::BindBuffers(rmesh->VAO, rmesh->vertices.id, rmesh->indices.id);

		material->shader->SetMat4("model", object->transform->GetGlobalMatrix());

		material->shader->SetBool("useTexture", material->BindTex());
		oglh::DrawElements(rmesh->indices.size);
		material->UnBindTex();
	}
	else
		LOGE("Object %s not find mesh %" SDL_PRIu64, object->GetName(), mesh);
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
	emesh = (r1Mesh*)App->resources->EGet(res);
	emesh->Attach();
}

void c1Mesh::OnInspector()
{
	if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen)) {
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

nlohmann::json c1Mesh::Parse()
{
	nlohmann::json ret = Component::Parse();

	if (!is_engine_mesh) {
		auto m = (r1Mesh*)App->resources->Get(mesh);
		ret["from_model"] = m->from_model;
		ret["uid"] = mesh;
	}

	return ret;
}

void c1Mesh::Unparse(const nlohmann::json& node)
{
	if (auto model = (r1Model*)App->resources->Get(node.value("from_model", 0ULL)))
		model->Attach();
	SetMesh(node.value("uid", 0ULL));
}
