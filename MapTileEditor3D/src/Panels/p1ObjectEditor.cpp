#include "p1ObjectEditor.h"
#include "Core/Application.h"
#include "Modules/m1Render3D.h"
#include "Tools/OpenGL/Viewport.h"

#include "Modules/m1Input.h"

#include "Tools/FileSystem.h"

#include "Resources/r1Shader.h"
#include "Resources/r1Texture.h"
#include "Modules/m1Resources.h"
#include "Tools/OpenGL/ObjectEditor.h"

#include "Tools/OpenGL/OpenGLHelper.h"

#include "ExternalTools/ImGui/imgui_internal.h"

#include "Tools/System/Logger.h"

p1ObjectEditor::p1ObjectEditor(bool start_active, bool appear_mainmenubar, bool can_close)
	: Panel("Object Editor", start_active, appear_mainmenubar, can_close, ICON_FA_DRAW_POLYGON /*other: https://fontawesome.com/icons/pencil-ruler?style=solid | https://fontawesome.com/icons/tree?style=solid | https://fontawesome.com/icons/hammer?style=solid*/)
{
}

p1ObjectEditor::~p1ObjectEditor()
{
	for (auto i = meshes.begin(); i != meshes.end(); ++i) {
		delete* i;
	}
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

	meshes.push_back(new ObjectEditor());

	//texture = (r1Texture*)App->resources->FindGet("tree01_lm2");
	//texture->Attach();
}

void p1ObjectEditor::Update()
{
	DrawObject();
	InfoWindow();
	ImGui::SameLine();
	ViewportWindow();
}

void p1ObjectEditor::DrawObject()
{
	viewport->Begin();
	oglh::EnableCullFace(false);
	oglh::DepthEnable(true);
	static r1Shader* shader = App->render->GetShader("default");
	shader->Use();
	if (texture != nullptr)
		texture->Bind();
	shader->SetBool("useTexture", texture != nullptr);
	for (auto i = meshes.begin(); i != meshes.end(); ++i) {
		(*i)->Draw(shader);
	}
	if (texture != nullptr)
		texture->Unbind();
	oglh::DepthEnable(false);
	oglh::EnableCullFace(true);
	viewport->End();
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

	if (meshes.empty()) {
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
			float width = ImGui::GetWindowContentRegionWidth();
			ImGui::BeginChild("imageChild", ImVec2(width, width));
			float height = texture->GetHeight() * width / texture->GetWidth();
			ImGui::Image((ImTextureID)texture->GetBufferID(), ImVec2(width, height), ImVec2(0, 0), ImVec2(1, -1));
			if (selected != -1 && selected < meshes.size()) {
				DrawUVs(width, height);
			}
			ImGui::EndChild();

			ImGui::BeginChild("meshesChild", { ImGui::GetWindowContentRegionWidth(),ImGui::GetWindowContentRegionWidth()*0.8f });
			auto i = meshes.begin();
			while (i != meshes.end())
			{
				ImGui::PushID(*i);
				if (ImGui::TreeNodeEx(*i, ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_NoTreePushOnOpen, (*i)->name.c_str())) {
					ImGui::Indent();

					if (ImGui::Selectable("Select", selected == i - meshes.begin()))
						selected = i - meshes.begin();

					strcpy((*i)->buffer, (*i)->name.c_str());
					ImGui::InputText("Name", (*i)->buffer, 20);
					(*i)->name.assign((*i)->buffer);
					ImGui::DragFloat3("Position", (*i)->position.ptr(), 0.01f);
					if (ImGui::DragFloat3("Rotation", (*i)->euler.ptr(), 1.f, -360.f, 360.f))
						(*i)->rot = Quat::FromEulerXYZ(math::DegToRad((*i)->euler.x), math::DegToRad((*i)->euler.y), math::DegToRad((*i)->euler.z));
					ImGui::DragFloat("Size", &(*i)->scale, 0.1f);

					ImGui::PushStyleColor(ImGuiCol_Button, { 1.f, 0.f, 0.f, 1.f });
					if (ImGui::Button("Delete")) {
						if (i - meshes.begin() == selected)
							selected = -1;
						i = meshes.erase(i);

					}
					else
						++i;

					ImGui::PopStyleColor();
					ImGui::Unindent();
					ImGui::PopID();
					continue;
				}
				++i;
				ImGui::PopID();
			}
			ImGui::EndChild();

			if (ImGui::Button("Create Mesh"))
				meshes.push_back(new ObjectEditor());
			ImGui::SameLine();
			if (ImGui::Button("Save")) {
				nlohmann::json object;
				for (auto i = meshes.begin(); i != meshes.end(); ++i) {
					nlohmann::json jm = (*i)->ToJson();
					object["meshes"].push_back(jm["mesh"]);
					for (int v = 0; v < 4*3; ++v) {
						object["vertices"].push_back(jm["object"]["vertices"][v]);
						if (v < 4 * 2)
							object["texCoords"].push_back(jm["object"]["texCoords"][v]);
					}
					
				}
				FileSystem::SaveJSONFile("Export/object.object", object);
			}
		}
	}

	ImGui::EndChild();
}

void p1ObjectEditor::DrawUVs(float width, float height)
{
	auto draw_list = ImGui::GetCurrentWindow()->DrawList;
	auto mesh = meshes[selected];
	auto uvs = mesh->uv.data;

	ImVec2 min = ImGui::GetItemRectMin() + ImVec2(uvs[0] * width, (1.f - uvs[1]) * height);
	ImVec2 max = ImGui::GetItemRectMin() + ImVec2(uvs[4] * width, (1.f - uvs[5]) * height);

	draw_list->AddRectFilled(min, max, ImGui::GetColorU32(ImVec4(0.f, 1.f, 0.f, 0.2f)));
	static float grab_width = 5.f;
	static ImVec2 grab_quad = ImVec2(grab_width, grab_width);
	static ImVec4 grab_color = ImVec4(1.f, 0.3f, 0.f, 1.f);
	std::vector<std::pair<ImVec2, ImVec2>> corners;
	corners.push_back({ min - grab_quad, min + grab_quad }); // 0 = bottom-left
	corners.push_back({ ImVec2(max.x, min.y) - grab_quad, ImVec2(max.x, min.y) + grab_quad }); // 1 = bottom-rigth
	corners.push_back({ max - grab_quad, max + grab_quad }); // 2 = top-right
	corners.push_back({ ImVec2(min.x, max.y) - grab_quad, ImVec2(min.x, max.y) + grab_quad }); // 3 = top_left
	for (auto i = corners.begin(); i != corners.end(); ++i)
		draw_list->AddRect((*i).first, (*i).second, ImGui::GetColorU32(grab_color));

	if (dragging == -1) {
		if (App->input->IsMouseButtonDown(SDL_BUTTON_LEFT)) {
			ImVec2 mouse = ImGui::GetMousePos();
			for (auto i = corners.begin(); i != corners.end(); ++i) {
				if (mouse.x > (*i).first.x && mouse.y > (*i).first.y && mouse.x < (*i).second.x && mouse.y < (*i).second.y) {
					dragging = i - corners.begin();
					break;
				}
			}
		}
		//TODO Change cursor
	}
	else {
		ImVec2 mouse = ImGui::GetMousePos();
		mouse = mouse - ImGui::GetItemRectMin();
		mouse = { mouse.x / width, 1.f - (mouse.y / height) };
		uvs[dragging * 2] = math::Clamp01<float>(mouse.x);
		uvs[dragging * 2 + 1] = math::Clamp01<float>(mouse.y);
		switch (dragging)
		{
		case 0:
			uvs[6] = uvs[0];
			uvs[3] = uvs[1];
			break;
		case 1:
			uvs[4] = uvs[2];
			uvs[1] = uvs[3];
			break;
		case 2:
			uvs[2] = uvs[4];
			uvs[7] = uvs[5];
			break;
		case 3:
			uvs[0] = uvs[6];
			uvs[5] = uvs[7];
			break;
		}
		mesh->size = float2(uvs[2] - uvs[0], uvs[7] - uvs[1]);
		oglh::BindBuffers(mesh->VAO, mesh->vertices.id, mesh->indices.id);
		oglh::BindTexture(mesh->uv.id);
		oglh::SetArrayBuffer(mesh->uv.id, mesh->uv.size, sizeof(float), 2, mesh->uv.data, 1, 2);
		oglh::UnBindTexture();
	}
	if (App->input->IsMouseButtonUp(SDL_BUTTON_LEFT)) {
		dragging = -1;
	}
}
