#include "Panels/p1Objects.h"
#include "Core/Application.h"
#include "Modules/m1Objects.h"
#include "Objects/Object.h"
#include "Modules/m1GUI.h"
#include "Modules/m1Events.h"
#include "Panels/p1Inspector.h"
#include "Modules/m1Resources.h"
#include "Panels/p1ObjectEditor.h"

#include "Resources/r1Object.h"
#include "Resources/r1Tileset3d.h"
#include "Resources/r1Texture.h"
#include "Resources/r1Model.h"
#include "Resources/r1Mesh.h"

#include "Modules/m1Render3D.h"
#include "Tools/OpenGL/Viewport.h"
#include "Tools/OpenGL/OpenGLHelper.h"
#include "Resources/r1Shader.h"

#include "Tools/System/Logger.h"

p1Objects::p1Objects(bool start_active, bool appear_mainmenubar, bool can_close)
	: Panel("Objects", start_active, appear_mainmenubar, can_close, ICON_FA_SITEMAP)
{
}

p1Objects::~p1Objects()
{
}

void p1Objects::Start()
{
	tileset = (r1Tileset3d*)App->resources->Get(11311841969679106682ULL); //TODO:
	tileset->Attach();

	viewport = App->render->CreateViewport("tileset3d editor");
	viewport->camera->frustum.SetVerticalFovAndAspectRatio(DegToRad(60.f), (float)size.x / (float)size.y);
	viewport->camera->is_active = false;
	viewport->camera->pan_mov = false;
	viewport->camera->rotation = Camera::RotationType::Orbit;
	viewport->camera->wasd_mov = false;
	viewport->camera->rf_mov = false;
	viewport->camera->LookAt(float3::zero);
}

void p1Objects::Update()
{
	float height = ImGui::GetContentRegionAvail().y;
	ImGui::BeginChild("Tree Objects", ImVec2(0.f, height*0.5f), true);
	Header();

	ImGui::Separator();

	ModalEditTileset();

	for (int i = 0; i < tileset->tiles.size(); ++i) {
		ImGui::PushID(i);
		if (ImGui::Selectable(tileset->tiles[i]->name.c_str(), selected == i)) selected = i;
		ImGui::PopID();
	}
	ImGui::EndChild();
}

void p1Objects::ModalEditTileset()
{
	if (ImGui::BeginPopupModal("Edit Tileset")) {
		{
			ImGui::BeginChild("EditTilesetInfo", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, ImGui::GetContentRegionAvail().y));
			{
				ImGui::BeginChild("TileList", ImVec2(ImGui::GetWindowWidth() * 0.3f, ImGui::GetWindowHeight() * 0.75f));
				for (int i = 0; i < tileset->tiles.size(); ++i) {
					ImGui::PushID(i);
					if (ImGui::Selectable(tileset->tiles[i]->name.c_str(), selected == i) && selected != i) {
						selected = i;
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

				if (ImGui::Button("Delete")) {
					tileset->tiles.erase(tileset->tiles.begin() + selected); //TODO: Destroy from memory
					delete t;
					selected = -1;
				}

				ImGui::NewLine();
				if (selected != -1)
					t->transform.OnInspector();

				ImGui::EndChild();
			}

			ImGui::Separator();
			static bool add3dMesh = false;
			if (ImGui::Button(ICON_FA_PLUS)) {
				ImGui::OpenPopup("Add3dMesh");
				ImGui::SetNextWindowCentered(ImVec2(600.f, 450.f));
				add3dMesh = true;
			}
			ImGui::SameLine();
			if (ImGui::Button("Save")) {
				tileset->GenerateFiles();
			}
			ImGui::SameLine();
			if (ImGui::Button("Exit")) {
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::BeginPopupModal("Add3dMesh", &add3dMesh)) {
				{
					ImGui::BeginChild("TilesetObjects", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, 0.f), true);
					ImGui::Text("Objects");
					ImGui::SameLine();
					auto models = App->resources->GetVectorOf(Resource::Type::Object);
					if (ImGui::Button("Add Missing")) {
						for (auto& i : models) {
							if (!IsOnTileset(i->GetUID())) {
								tileset->tiles.push_back(new r1Tileset3d::Tile3d(i->GetUID(), i->name));
								i->Attach();
							}
						}
						ImGui::CloseCurrentPopup();
					}
					ImGui::Separator();
					for (auto& i : models) {
						ImGui::PushID(i);
						if (ImGui::Button(i->name.c_str())) {
							tileset->tiles.push_back(new r1Tileset3d::Tile3d(i->GetUID(), i->name));
							i->Attach();
							ImGui::PopID();
							ImGui::CloseCurrentPopup();
							break;
						}
						ImGui::PopID();
					}
					ImGui::EndChild();
				}
				ImGui::SameLine();
				{
					ImGui::BeginChild("TilesetTerrains", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, 0.f), true);
					ImGui::Text("Terrains");
					ImGui::Separator();
					auto models = App->resources->GetVectorOf(Resource::Type::Model);
					for (auto& i : models) {
						if (!((r1Model*)i)->isTerrain)
							continue;
						ImGui::PushID(i);
						if (ImGui::Button(i->name.c_str())) {
							tileset->tiles.push_back(new r1Tileset3d::Tile3d(i->GetUID(), i->name));
							i->Attach();
							ImGui::PopID();
							ImGui::CloseCurrentPopup();
							break;
						}
						ImGui::PopID();
					}
					ImGui::EndChild();
				}

				ImGui::EndPopup();
			}

			ImGui::EndChild();
		}
		ImGui::SameLine();

		ImGui::BeginChild("EditTilesetViewport", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, ImGui::GetContentRegionAvail().y));

		if (selected > -1 && selected < tileset->tiles.size()) {
			viewport->Begin();

			Resource* obj = (Resource*)App->resources->Get(tileset->tiles[selected]->uidObject);
			if (obj != nullptr) {
				if (obj->GetType() == Resource::Type::Model) {
					r1Model* model = static_cast<r1Model*>(obj);

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
				else if (obj->GetType() == Resource::Type::Object) {
					r1Object* model = static_cast<r1Object*>(obj);

					static auto shader = App->render->GetShader("default");
					shader->Use();

					oglh::BindBuffers(model->VAO, model->bvertices.id, model->bindices.id);
					shader->SetMat4("model", tileset->tiles[selected]->transform.GetGlobalMatrix());

					if (model->texture != nullptr) model->texture->Bind();
					shader->SetBool("useTexture", model->btexture.data != nullptr);
					oglh::DrawElements(model->bindices.size);
					if (model->texture != nullptr) model->texture->Unbind();
				}
			}

			viewport->End();
		}


		viewport->RenderOnImGui();

		ImGui::EndChild();

		ImGui::EndPopup();
	}
}

bool p1Objects::IsOnTileset(const uint64_t& uid) const {
	for (auto& t : tileset->tiles) {
		if (t->uidObject == uid)
			return true;
	}
	return false;
}

void p1Objects::Header()
{
	if (App->objects->layer_root_selected != nullptr) {
		for (auto i = App->objects->layer_root_selected->children.begin(); i != App->objects->layer_root_selected->children.end(); ++i)
			TreeNode(*i);
	}

	for (auto i = to_destroy.begin(); i != to_destroy.end(); ++i)
		App->objects->DeleteObject(*i);
	to_destroy.clear();
	ImGui::EndChild();

	ImGui::BeginChild("Objects", ImVec2(0.f, 0.f), true);
	ImGui::Text("Objects");
	ImGui::SameLine(100.f);
	if (ImGui::Button("Edit Objects")) {
		App->gui->object_editor->SetActive(true);
	}
	if (ImGui::Button("Edit Tileset")) {
		ImGui::OpenPopup("Edit Tileset");
		ImGui::SetNextWindowCentered(ImVec2(850.f, 600.f));
	}
}

uint64_t p1Objects::GetObjectSelected() const
{
	if (objSelected != nullptr)
		return objSelected->GetUID();
	return 0ULL;
}

void p1Objects::TreeNode(Object* obj)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	if (App->objects->selected == obj)
		flags |= ImGuiTreeNodeFlags_Selected;
	if (obj->children.empty())
		flags |= ImGuiTreeNodeFlags_Leaf;

	bool tree = ImGui::TreeNodeEx(obj->GetName(), flags);
	ImGui::PushID(obj);
	if (ImGui::IsItemClicked() && App->objects->selected != obj) {
		App->objects->selected = obj;
		App->gui->inspector->SetSelected(obj, p1Inspector::SelectedType::OBJECT);
	}

	ImGui::OpenPopupOnItemClick("popup_gameobject", ImGuiMouseButton_Right);

	if (ImGui::BeginPopup("popup_gameobject")) {
		if (ImGui::MenuItem("Create Empty")) {
			App->objects->CreateEmptyObject(obj);
		}
		if (ImGui::MenuItem("Delete")) {
			to_destroy.push_back(obj);
			if (App->objects->selected == obj) {
				App->objects->selected = nullptr;
				App->gui->inspector->SetSelected(nullptr, p1Inspector::SelectedType::NONE);
			}
		}

		ImGui::EndPopup();
	}

	if (tree) {
			for (auto i = obj->children.begin(); i != obj->children.end(); ++i) {
				TreeNode(*i);
			}
		ImGui::TreePop();
	}
	ImGui::PopID();
}
