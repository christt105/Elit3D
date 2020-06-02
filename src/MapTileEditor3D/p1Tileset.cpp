#include "p1Tileset.h"
#include "Application.h"
#include "m1Resources.h"
#include "FileSystem.h"

#include "r1Texture.h"
#include "r1Tileset.h"

#include "ExternalTools/ImGui/imgui_internal.h"

#include "Logger.h"

p1Tileset::p1Tileset(bool start_enabled) : Panel("Tileset", start_enabled, ICON_FA_PALETTE)
{
}

p1Tileset::~p1Tileset()
{
}

void p1Tileset::Update()
{
	static bool modal = false;
	ImGui::Spacing();
	if (ImGui::Button("Select Tileset")) { // TODO: Center in window
		if (!select_tileset)
			select_tileset = true;
	}
	if (tileset != 0ULL) {
		r1Tileset* tile = (r1Tileset*)App->resources->Get(tileset);
		if (tile != nullptr) {
			r1Texture* texture = (r1Texture*)App->resources->Get(tile->GetTextureUID());
			if (texture != nullptr)
				ImGui::Image((ImTextureID)texture->GetBufferID(), ImVec2(texture->GetWidth(), texture->GetHeight()), ImVec2(0, 0), ImVec2(1, -1));
		}
	}
	
	if (select_tileset) {
		if (ImGui::Begin("Choose Tileset", &select_tileset)) {
			if (ImGui::Button("+ Create Tileset")) {
				modal = true;

				data.tile_size[0] = data.tile_size[1] = 32;
				data.tile_size[2] = data.tile_size[3] = 0;
				data.transparent_color[0] = data.transparent_color[1] = data.transparent_color[2] = 1.f;
				memset(data.buf_name, 0, 25);
				data.imageUID = 0ULL;

				ImGui::OpenPopup("Create Tileset");
			}

			ImGui::Separator();

			if (modal)
				ImGui::SetNextWindowSize(ImVec2(450.f, 250.f));

			if (ImGui::BeginPopupModal("Create Tileset", &modal)) {
				ModalCreateTileset(modal);
			}
			auto vector = App->resources->GetVectorOf(Resource::Type::Tileset);

			for (auto i = vector.begin(); i != vector.end(); ++i) {
				if (ImGui::Button((*i)->name.c_str())) {
					select_tileset = false;
					tileset = (*i)->GetUID();
					(*i)->Attach();
				}
			}

			ImGui::End();
		}
	}
}

void p1Tileset::ModalCreateTileset(bool& modal)
{
	ImGui::InputText("Name", data.buf_name, 25);
	ImGui::Separator();

	if (data.imageUID != 0ULL) {
		r1Texture* tex = (r1Texture*)App->resources->Get(data.imageUID);
		if (tex != nullptr) {
			ImGui::Text("Current Texture: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.f, 0.6f, 0.f, 1.f), "%s", tex->name.c_str());
			ImGui::SameLine();
		}
	}
	if (ImGui::Button("Select Image")) {
		ImGui::OpenPopup("SelectImageTileset");
		auto vec = App->resources->GetVectorOf(Resource::Type::Texture);
		for (auto i = vec.begin(); i != vec.end(); ++i) {
			(*i)->Attach();
		}
	}

	bool open = true;
	bool select = ImGui::BeginPopupModal("SelectImageTileset", &open);

	if (select) {
		ModalSelectImageTileset();
	}

	ImGui::Checkbox("Use transparent color", &data.transparent); ImGui::SameLine();
	ImGui::ColorEdit3("##transparentColor", data.transparent_color, ImGuiColorEditFlags_NoInputs);

	float width = 150.f;
	ImGui::SetNextItemWidth(width);
	ImGui::InputInt("Tile width", data.tile_size);
	ImGui::SetNextItemWidth(width);
	ImGui::InputInt("Tile height", &data.tile_size[1]);
	ImGui::SetNextItemWidth(width);
	ImGui::InputInt("Margin", &data.tile_size[2]);
	ImGui::SetNextItemWidth(width);
	ImGui::InputInt("Spacing", &data.tile_size[3]);

	bool pushStyle = false;
	if (strlen(data.buf_name) == 0 || data.imageUID == 0ULL) {
		pushStyle = true;
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}
	if (ImGui::Button("Create")) {
		nlohmann::json jsontileset;

		if (!App->file_system->Exists("Assets/Tilesets"))
			App->file_system->CreateFolder("Assets/Tilesets/");

		jsontileset["Image"] = data.imageUID;
		jsontileset["tile"]["width"] = data.tile_size[0];
		jsontileset["tile"]["height"] = data.tile_size[1];
		jsontileset["tile"]["margin"] = data.tile_size[2];
		jsontileset["tile"]["spacing"] = data.tile_size[3];

		std::string path("Assets/Tilesets/" + std::string(data.buf_name) + ".tileset");
		App->file_system->SaveJSONFile(path.c_str(), jsontileset);
		uint64_t meta = App->resources->GenerateMeta(path.c_str());
		r1Tileset* res = App->resources->CreateResource<r1Tileset>(path.c_str(), meta);
		res->GenerateFiles();
		tileset = res->GetUID();
		res->Attach();

		ImGui::CloseCurrentPopup();
		select_tileset = false;
	}

	if (pushStyle) {
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}

	ImGui::SameLine();
	if (ImGui::Button("Cancel")) {
		modal = false;
		ImGui::CloseCurrentPopup();
	}

	ImGui::EndPopup();
}

void p1Tileset::ModalSelectImageTileset()
{
	auto vec = App->resources->GetVectorOf(Resource::Type::Texture);
	for (auto i = vec.begin(); i != vec.end(); ++i) {
		if ((i - vec.begin()) % 2 != 0)
			ImGui::SameLine();

		if (ImGui::ImageButton((ImTextureID)((r1Texture*)(*i))->GetBufferID(), ImVec2(100.f, 100.f), ImVec2(0, 0), ImVec2(1, -(float)((r1Texture*)(*i))->GetWidth() / (float)((r1Texture*)(*i))->GetHeight()))) {
			data.imageUID = (*i)->GetUID();

			for (auto j = vec.begin(); j != vec.end(); ++j) {
				(*j)->Detach();
			}

			(*i)->Attach();

			ImGui::CloseCurrentPopup();
			break;
		}
	}

	ImGui::EndPopup();
}
