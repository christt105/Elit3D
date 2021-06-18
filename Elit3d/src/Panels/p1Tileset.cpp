#include "Panels/p1Tileset.h"
#include "Core/Application.h"
#include "Modules/m1Resources.h"
#include "Tools/FileSystem.h"
#include "Modules/m1Render3D.h"
#include "Resources/r1Shader.h"

#include "Resources/r1Texture.h"
#include "Resources/r1Tileset.h"
#include "Resources/r1Shader.h"

#include "ExternalTools/ImGui/imgui_internal.h"

#include "Tools/System/Logger.h"

p1Tileset::p1Tileset(bool start_enabled, bool appear_mainmenubar, bool can_close)
	: Panel("Tileset", start_enabled, appear_mainmenubar, can_close, ICON_FA_PALETTE)
{
}

p1Tileset::~p1Tileset()
{
}

void p1Tileset::Start()
{
}

void p1Tileset::Update()
{
	static bool modal = false;

	//TODO: Multiple tilesets
	
	if (tileset == 0ULL) {
		if (ImGui::Button("Select Tileset")) { // TODO: Center in window
			if (!select_tileset)
				select_tileset = true;
		}
	}
	else {
		if (ImGui::Button("Change Tileset")) { // TODO: Center in window
			if (!select_tileset)
				select_tileset = true;
		}
		ImGui::Separator();

		r1Tileset* tile = (r1Tileset*)App->resources->Get(tileset);
		if (tile != nullptr) {
			TileSetInfo(tile);

			ImGui::Separator();

			r1Texture* texture = (r1Texture*)App->resources->Get(tile->GetTextureUID());
			if (texture != nullptr) {
				DisplayImage(texture, tile);
			}
		}
	}
	
	if (select_tileset) {
		if (ImGui::Begin("Choose Tileset", &select_tileset)) {
			if (ImGui::Button("+ Create Tileset")) {
				modal = true;

				data.Reset();

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
					SelectTileset((*i)->GetUID());
				}
				ImGui::SameLine();
				ImGui::PushID(*i);
				if (ImGui::Button(ICON_FA_TRASH)) {
					App->resources->DeleteResource((*i)->GetUID());
				}
				ImGui::PopID();
			}

			ImGui::End();
		}
	}
}

void p1Tileset::DisplayImage(r1Texture* texture, r1Tileset* tile)
{
	if (ImGui::BeginChild("##tileset")) {
		float width = ImGui::GetContentRegionAvailWidth() - 10.f;
		float height = (float)texture->GetHeight() * width / (float)texture->GetWidth();
		float2 tileSize{ ((float)tile->width * width/(float)texture->GetWidth()), ((float)tile->height * height/(float)texture->GetHeight()) };
		ImGui::Image((ImTextureID)texture->GetBufferID(), ImVec2(width, height), ImVec2(0, 0), ImVec2(1, -1));
		if (ImGui::IsItemHovered()) {
			ImVec2 mouse = ImGui::GetMousePos() - ImGui::GetItemRectMin();
			ImVec2 tile_mouse = ImVec2(floor(mouse.x / tileSize.x), floor(mouse.y / tileSize.y));

			ImGui::BeginTooltip();
			ImGui::Text("Tile: %i, %i", (int)tile_mouse.x, (int)tile_mouse.y);
			ImGui::EndTooltip();

			ImVec2 min = ImGui::GetItemRectMin() + ImVec2(tile_mouse.x * tileSize.x, tile_mouse.y * tileSize.y);
			ImVec2 max = min + ImVec2(tileSize.x, tileSize.y);

			auto draw_list = ImGui::GetCurrentWindow()->DrawList;

			draw_list->AddRectFilled(min, max, ImGui::GetColorU32(ImVec4(0.8f, 0.8f, 0.8f, 0.6f)));
			draw_list->AddRect(min, max, ImGui::GetColorU32(ImVec4(0.141f, 0.701f, 1.f, 1.f)));

			if (ImGui::IsItemClicked()) {
				tile_selected[0] = (int)tile_mouse.x;
				tile_selected[1] = (int)tile_mouse.y;
			}

		}
		if (tile_selected[0] != -1) {
			auto draw_list = ImGui::GetCurrentWindow()->DrawList;

			ImVec2 min = ImGui::GetItemRectMin() + ImVec2((float)tile_selected[0] * tileSize.x, (float)tile_selected[1] * tileSize.y);
			ImVec2 max = min + ImVec2(tileSize.x, tileSize.y);

			draw_list->AddRectFilled(min, max, ImGui::GetColorU32(ImVec4(0.8f, 0.8f, 0.8f, 0.3f)));
			draw_list->AddRect(min, max, ImGui::GetColorU32(ImVec4(1.f, 0.654f, 0.141f, 1.f)));
		}
		ImGui::EndChild();
	}
}

void p1Tileset::TileSetInfo(r1Tileset* tile)
{
	auto tex = App->resources->Get(tile->texture_uid);
	ImGui::TextColored(ImVec4(1.f, 0.6f, 0.6f, 1.f), tile->name.c_str());
	ImGui::Spacing();
	if (tex == nullptr) {
		//MLOGW("Deleted texture of the tileset. Add a texture on the panel inspector");
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.f, 0.f, 1.f));
		ImGui::TextWrapped("Image of the tileset deleted. Edit tileset with an existing image.");
		ImGui::PopStyleColor();
		return;
	}
	ImGui::Text("Image: "); ImGui::SameLine(); 
	ImGui::TextColored(ImVec4(1.f, 0.6f, 0.6f, 1.f), tex->path.c_str());
	ImGui::Text("Tile width:  ");  ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 0.6f, 0.6f, 1.f), "%i", tile->width);
	ImGui::SameLine();
	ImGui::Text("\tMargin: ");  ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 0.6f, 0.6f, 1.f), "%i", tile->margin);
	ImGui::Text("Tile height: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 0.6f, 0.6f, 1.f), "%i", tile->height);
	ImGui::SameLine();
	ImGui::Text("\tSpacin: ");  ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 0.6f, 0.6f, 1.f), "%i", tile->spacing);

	ImGui::Text("Columns:\t");  ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 0.6f, 0.6f, 1.f), "%i", tile->columns);
	ImGui::Text("Rows:\t"); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 0.6f, 0.6f, 1.f), "%i", tile->ntiles / tile->columns);
}

bool p1Tileset::SelectTex()
{
	if (tileset != 0) {
		auto res = (r1Tileset*)App->resources->Get(tileset);
		if (res) {
			auto tex = (r1Texture*)App->resources->Get(res->GetTextureUID());
			if (tex) {
				tex->Bind();
				return true;
			}
		}
	}
	return false;
}

void p1Tileset::SelectTileset(const uint64_t& uid)
{
	if (tileset != uid) {
		if (tileset != 0ULL) {
			auto t = (r1Tileset*)App->resources->Get(tileset);
			if (t != nullptr)
				t->Detach();
		}
		tileset = uid;
		auto t = (r1Tileset*)App->resources->Get(tileset);
		if (t != nullptr) {
			t->Attach();

			auto shader = App->render->GetShader("tilemap");
			shader->Use();
			shader->SetInt2("ntilesAtlas", { t->columns, t->ntiles / t->columns });
			shader = App->render->GetShader("selectTile");
			shader->Use();
			shader->SetInt2("ntilesAtlas", { t->columns, t->ntiles / t->columns });
		}
	}
}

void p1Tileset::SelectTransparentColor(r1Shader*& shader)
{
	if (tileset != 0) {
		auto res = (r1Tileset*)App->resources->Get(tileset);
		if (res) {
			shader->SetBool("useTransparent", res->use_transparent);
			if (res->use_transparent) {
				shader->SetVec3("transparentColor", float3(res->transparent_color[0], res->transparent_color[1], res->transparent_color[2]));
			}
		}
	}
}

void p1Tileset::SetColumnUniform(r1Shader*& shader)
{
	if (tileset != 0) {
		auto res = (r1Tileset*)App->resources->Get(tileset);
		if (res) {
			shader->SetInt("max_columns", res->columns);
		}
	}
}

int2 p1Tileset::GetTileSelected() const
{
	int2 ret = { tile_selected[0], tile_selected[1] };
	auto tile = (r1Tileset*)App->resources->Get(tileset);
	if (tile)
		ret.y = tile->ntiles / tile->columns - ret.y - 1;
	return ret;
}

TILE_DATA_TYPE p1Tileset::GetTileIDSelected() const
{
	if (tile_selected[0] != -1 || tile_selected[1] != -1) {
		int2 ret = { tile_selected[0], tile_selected[1] };
		auto tile = (r1Tileset*)App->resources->Get(tileset);
		if (tile) {
			ret.y = tile->ntiles / tile->columns - ret.y - 1;

			return tile->columns * ret.y + ret.x;
		}
	}
	return (TILE_DATA_TYPE)INVALID_TILE;
}

void p1Tileset::SetTileIDSelected(TILE_DATA_TYPE id)
{
	auto tile = (r1Tileset*)App->resources->Get(tileset);
	if (tile) {
		tile_selected[0] = id % tile->columns;
		tile_selected[1] = (id / tile->columns - tile->ntiles/tile->columns + 1) * -1;
	}
}

int2 p1Tileset::GetTileByID(TILE_DATA_TYPE id) const
{
	auto tile = (r1Tileset*)App->resources->Get(tileset);
	if (tile) {
		return int2( id % tile->columns, (id / tile->columns - tile->ntiles / tile->columns + 1) * -1 );
	}
	return { 0, 0 };
}

int p1Tileset::GetTileWidth() const
{
	auto t = (r1Tileset*)App->resources->Get(tileset);
	if (t) {
		return t->width;
	}
	return 0;
}

int p1Tileset::GetTileHeight() const
{
	auto t = (r1Tileset*)App->resources->Get(tileset);
	if (t) {
		return t->height;
	}
	return 0;
}

const uint64_t& p1Tileset::GetTileset() const
{
	return tileset;
}

int2 p1Tileset::GetTilesetSize() const
{
	auto t = (r1Tileset*)App->resources->Get(tileset);
	if (t != nullptr) {
		return { t->columns, t->ntiles / t->columns };
	}
	return int2();
}

void p1Tileset::ModalCreateTileset(bool& modal)
{
	ImGui::InputText("Name", data.buf_name, 25);
	ImGui::Separator();

	if (data.imageUID != 0ULL) {
		const r1Texture* tex = (r1Texture*)App->resources->Get(data.imageUID);
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
	ImGui::InputInt("Tile width", &data.tile_size[0]);
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

		if (!FileSystem::Exists("Assets/Tilesets"))
			FileSystem::CreateFolder("Assets/Tilesets/");

		jsontileset["Image"] = data.imageUID;

		r1Texture* tex = (r1Texture*)App->resources->Get(data.imageUID);
		tex->Attach();

		jsontileset["columns"] = (int)floor(tex->GetWidth() / data.tile_size[0]);
		jsontileset["ntiles"] = (int)floor(tex->GetHeight() / data.tile_size[1]) * jsontileset["columns"];

		tex->Detach();

		jsontileset["tile"]["width"] = data.tile_size[0];
		jsontileset["tile"]["height"] = data.tile_size[1];
		jsontileset["tile"]["margin"] = data.tile_size[2];
		jsontileset["tile"]["spacing"] = data.tile_size[3];

		jsontileset["use transparent"] = data.transparent;
		jsontileset["transparent color"]["r"] = data.transparent_color[0];
		jsontileset["transparent color"]["g"] = data.transparent_color[1];
		jsontileset["transparent color"]["b"] = data.transparent_color[2];

		std::string path("Assets/Tilesets/" + std::string(data.buf_name) + ".tileset");
		App->resources->PauseFileWatcher(true);
		FileSystem::SaveJSONFile(path.c_str(), jsontileset);
		uint64_t meta = App->resources->GenerateMeta(path.c_str());
		r1Tileset* res = App->resources->CreateResource<r1Tileset>(path.c_str(), meta);
		App->resources->PauseFileWatcher(false);
		res->LoadVars();
		
		SelectTileset(res->GetUID());

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
		if (!((r1Texture*)(*i))->tileset)
			continue;

		ImGui::PushID(*i);
		ImGui::BeginGroup();
		if (ImGui::Selectable("##tilesetimage")) {
			data.imageUID = (*i)->GetUID();

			for (auto j = vec.begin(); j != vec.end(); ++j) {
				(*j)->Detach();
			}

			(*i)->Attach();

			ImGui::CloseCurrentPopup();
			ImGui::EndGroup();
			ImGui::PopID();
			break;
		}
		ImGui::SameLine();
		ImGui::Image((ImTextureID)((r1Texture*)(*i))->GetBufferID(), 
			ImVec2(100.f, 100.f), ImVec2(0, 0), 
			ImVec2(1, -1));
		ImGui::SameLine();
		ImGui::Text((*i)->name.c_str());
		ImGui::EndGroup();
		ImGui::PopID();
	}

	ImGui::EndPopup();
}
