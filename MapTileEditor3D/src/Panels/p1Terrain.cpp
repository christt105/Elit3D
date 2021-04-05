#include "p1Terrain.h"
#include "Core/Application.h"
#include "Modules/m1Render3D.h"
#include "Modules/m1Resources.h"

#include "Resources/r1Model.h"
#include "Resources/r1Mesh.h"
#include "Resources/r1Texture.h"
#include "Resources/r1Tileset3d.h"

#include "Resources/r1Shader.h"
#include "Tools/OpenGL/Viewport.h"
#include "Tools/Camera.h"

#include "Tools/OpenGL/OpenGLHelper.h"

#include "ExternalTools/mmgr/mmgr.h"

p1Terrain::p1Terrain(bool start_enabled, bool appear_mainmenubar, bool can_close) : Panel("Terrain", start_enabled, appear_mainmenubar, can_close, ICON_FA_MOUNTAIN)
{
}

void p1Terrain::Start()
{
	tileset = (r1Tileset3d*)App->resources->Get(11311841969679106682);

	viewport = App->render->CreateViewport("tileset3d editor");
	viewport->camera->is_active = false;
	viewport->camera->pan_mov = false;
	viewport->camera->rotation = Camera::RotationType::Orbit;
	viewport->camera->wasd_mov = false;
	viewport->camera->rf_mov = false;
	viewport->camera->LookAt(float3::zero);
}

void p1Terrain::Update()
{
	if (ImGui::Button("Edit Tileset")) {
		ImGui::OpenPopup("Edit Tileset");
		ImGui::SetNextWindowCentered(ImVec2(850.f, 600.f));
	}

	if (ImGui::BeginPopupModal("Edit Tileset")) {
		{
			ImGui::BeginChild("EditTilesetInfo", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, ImGui::GetContentRegionAvail().y));
			{
				ImGui::BeginChild("TileList", ImVec2(ImGui::GetWindowWidth() * 0.3, ImGui::GetWindowHeight() * 0.75));
				for (int i = 0; i < tileset->tiles.size(); ++i) {
					ImGui::PushID(i);
					if (ImGui::Selectable(tileset->tiles[i]->name.c_str(), selected == i) && selected != i) {
						if (selected > -1 && selected < tileset->tiles.size())
							if (auto r = App->resources->Get(tileset->tiles[selected]->uidObject))
								r->Detach();
						selected = i;
						if (auto r = App->resources->Get(tileset->tiles[i]->uidObject)) {
							r->Attach();
						}
						strcpy_s(bufname, sizeof(char) * 30, tileset->tiles[i]->name.c_str());
					}
					ImGui::PopID();
				}
				ImGui::EndChild();
			}

			if (selected > -1 && selected < tileset->tiles.size()) {
				ImGui::SameLine();

				ImGui::BeginChild("TileInfo", ImVec2(ImGui::GetWindowWidth() * 0.7f, ImGui::GetWindowHeight() * 0.75f));
				r1Tileset3d::Tile3d* t = tileset->tiles[selected];

				if (ImGui::InputText("Name", bufname, 30))
					t->name = bufname;

				ImGui::Text("From Resource:\t");
				ImGui::SameLine();
				if (auto r = App->resources->Get(t->uidObject)) {
						ImGui::Text((r->name + "." + r->extension).c_str());
				}
				else {
					ImGui::Text("Unknown");
				}

				ImGui::NewLine();
				
				t->transform.OnInspector();

				ImGui::EndChild();
			}

			ImGui::Separator();
			if (ImGui::Button(ICON_FA_PLUS)) {
				ImGui::OpenPopup("Add3dMesh");
			}
			ImGui::SameLine();
			if (ImGui::Button("Save")) {
				tileset->GenerateFiles();
			}
			ImGui::SameLine();
			if (ImGui::Button("Exit")) {
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::BeginPopupModal("Add3dMesh")) {
				auto models = App->resources->GetVectorOf(Resource::Type::Model);

				for (auto& i : models) {
					if (!((r1Model*)i)->isTerrain)
						continue;
					ImGui::PushID(i);
					if (ImGui::Button(i->name.c_str())) {
						tileset->tiles.push_back(new r1Tileset3d::Tile3d(i->GetUID(), i->name));
						ImGui::PopID();
						ImGui::CloseCurrentPopup();
						break;
					}
					ImGui::PopID();
				}

				ImGui::EndPopup();
			}

			ImGui::EndChild();
		}
		ImGui::SameLine();

		ImGui::BeginChild("EditTilesetViewport", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, ImGui::GetContentRegionAvail().y));

		if (selected > -1 && selected < tileset->tiles.size()) {
			viewport->Begin();

			r1Model* model = (r1Model*)App->resources->Get(tileset->tiles[selected]->uidObject);
			if (model != nullptr) {
				static auto shader = App->render->GetShader("default");
				shader->Use();

				for (auto& m : model->meshes) {
					oglh::BindBuffers(m->VAO, m->vertices.id, m->indices.id);

					shader->SetMat4("model", tileset->tiles[selected]->transform.GetGlobalMatrix());

					auto tex = (r1Texture*)App->resources->Get(model->materials[m->tex_i]);
					if (tex != nullptr) tex->Bind();
					shader->SetBool("useTexture", m->texture.data != nullptr);
					oglh::DrawElements(m->indices.size);
					if (tex != nullptr) tex->Unbind();
				}
			}

			viewport->End();
		}


		viewport->RenderOnImGui();

		ImGui::EndChild();

		ImGui::EndPopup();
	}

	for (int i = 0; i < tileset->tiles.size(); ++i) {
		ImGui::PushID(i);
		if (ImGui::Selectable(tileset->tiles[i]->name.c_str(), selected == i)) selected = i;
		ImGui::PopID();
	}
}
