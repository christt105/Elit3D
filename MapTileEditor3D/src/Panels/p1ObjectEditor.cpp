#include "p1ObjectEditor.h"
#include "Core/Application.h"
#include "Modules/m1Render3D.h"
#include "Tools/OpenGL/Viewport.h"

#include "Modules/m1Objects.h"
#include "Objects/Object.h"
#include "Objects/Components/c1Mesh.h"
#include "Objects/Components/c1Material.h"
#include "Resources/r1Mesh.h"
#include "Resources/r1Texture.h"

#include "Tools/OpenGL/OpenGLHelper.h"

#include "ExternalTools/ImGui/imgui_internal.h"

#include "Tools/System/Logger.h"

p1ObjectEditor::p1ObjectEditor(bool start_active, bool appear_mainmenubar, bool can_close)
	: Panel("Object Editor", start_active, appear_mainmenubar, can_close, ICON_FA_DRAW_POLYGON /*other: https://fontawesome.com/icons/pencil-ruler?style=solid | https://fontawesome.com/icons/tree?style=solid | https://fontawesome.com/icons/hammer?style=solid*/)
{
}

p1ObjectEditor::~p1ObjectEditor()
{
	App->objects->DeleteObject(object);
}

void p1ObjectEditor::Start()
{
	viewport = App->render->CreateViewport("object editor");
	viewport->camera->is_active = false;
	viewport->camera->pan_mov = false;
	viewport->camera->rotation = Camera::RotationType::Orbit;
	viewport->camera->wasd_mov = false;
	viewport->camera->rf_mov = false;
	viewport->camera->LookAt(float3::zero);

	object = App->objects->CreateEmptyObject();
	
	r1Mesh* mesh = App->resources->CreateResource<r1Mesh>("");
	mesh->vertices.size = 4; //Quad
	mesh->vertices.data = new float[mesh->vertices.size * 3]{
		0.f, 0.f, 0.f,
		1.f, 0.f, 0.f,
		1.f, 1.f, 0.3f,
		0.f, 1.f, 0.3f
	};
	mesh->indices.size = 6;
	mesh->indices.data = new unsigned int[mesh->indices.size]{
		0, 2, 1,
		0, 3, 2
	};
	mesh->texture.size = mesh->vertices.size;
	mesh->texture.data = new float[mesh->texture.size * 2]{
		0.f, 0.f,
		1.f, 0.f,
		1.f, 1.f,
		0.f, 1.f
	};

	mesh->Attach();

	oglh::GenVAO(mesh->VAO);
	oglh::GenArrayBuffer(mesh->vertices.id, mesh->vertices.size, sizeof(float), 3, mesh->vertices.data, 0, 3);
	oglh::GenElementBuffer(mesh->indices.id, mesh->indices.size, mesh->indices.data);
	oglh::GenArrayBuffer(mesh->texture.id, mesh->texture.size, sizeof(float), 2, mesh->texture.data, 1, 2);

	c1Mesh* cmesh = object->CreateComponent<c1Mesh>();
	cmesh->SetMesh(mesh->GetUID());

	auto mat = object->GetComponent<c1Material>();
	texture = (r1Texture*)App->resources->FindGet("tree01_lm2");
	mat->SetTexture(texture);
}

void p1ObjectEditor::Update()
{
	viewport->Begin();
	oglh::EnableCullFace(false);
	object->Update();
	oglh::EnableCullFace(true);
	viewport->End();

	InfoWindow();
	ImGui::SameLine();
	ViewportWindow();
}

void p1ObjectEditor::ViewportWindow()
{
	ImGui::BeginChild("childObjectViewport", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.7f, 0.f));

	viewport->camera->is_active = ImGui::IsWindowHovered();

	ImGui::PushClipRect(ImGui::GetWindowPos(), ImGui::GetWindowPos() + ImGui::GetWindowSize(), false);
	ImGui::SetCursorScreenPos(ImGui::GetWindowPos());

	ImVec2 window_size = ImGui::GetContentRegionAvail();
	viewport->UpdateSize((int)window_size.x, (int)window_size.y); // TODO: Extract to a viewport function

	viewport->Update();

	viewport->Blit();

	ImGui::Image((ImTextureID)viewport->GetTexture(), window_size, ImVec2(0, 0), ImVec2(1, -1));

	ImGui::PopClipRect();

	ImGui::EndChild();
}

void p1ObjectEditor::InfoWindow()
{
	ImGui::BeginChild("childObjectInfo", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.3f, 0.f), false, ImGuiWindowFlags_NoMove);

	if (object == nullptr) {
		if (ImGui::Button("Create")) {
			ImGui::OpenPopup("CreateObject");
			ImGui::SetNextWindowCentered(ImVec2(500.f, 300.f));
		}
		if (ImGui::Button("Load")) {

		}
		if (ImGui::BeginPopupModal("CreateObject")) {
			static char buf[35]{ "" };
			ImGui::InputText("Name", buf, 35);
			if (ImGui::Button("Create")) {

			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel")) {
				memset(buf, 0, 35);
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}
	else {
		if (texture == nullptr) {
			ImGui::Button("Load Texture");
		}
		else {
			ImGui::BeginChild("imageChild", ImVec2(ImGui::GetWindowContentRegionWidth(), ImGui::GetWindowContentRegionWidth()));
			ImGui::Image((ImTextureID)texture->GetBufferID(), ImVec2(ImGui::GetWindowContentRegionWidth(), texture->GetHeight() * ImGui::GetWindowContentRegionWidth() / texture->GetWidth()), ImVec2(0, 0), ImVec2(1, -1));
			ImGui::EndChild();

			auto comp = object->GetComponents();
			for (auto i = comp.begin(); i != comp.end(); ++i)
			{
				if((*i)->GetType() == Component::Type::Mesh)
					ImGui::Text("%" PRIu64, ((c1Mesh*)(*i))->GetMesh());
			}
		}
	}

	ImGui::EndChild();
}
