#include "r1Tileset.h"
#include "Application.h"
#include "m1Resources.h"
#include "FileSystem.h"
#include "r1Texture.h"

#include "Logger.h"

#include "ExternalTools/mmgr/mmgr.h"

r1Tileset::r1Tileset(const uint64_t& _uid) : Resource(Resource::Type::Tileset, _uid)
{
}

r1Tileset::~r1Tileset()
{
}

void r1Tileset::Load()
{
	r1Texture* res = (r1Texture*)App->resources->Get(texture_uid);
	if (res != nullptr)
		res->Attach();
	else
		LOGW("Texture with id %s not found in tileset %s(%s)", std::to_string(texture_uid).c_str(), name.c_str(), std::to_string(uid).c_str());
}

void r1Tileset::LoadVars()
{
	auto json = FileSystem::OpenJSONFile(path.c_str());

	texture_uid = json.value("Image", 0ULL);

	width = json["tile"].value("width", 32);
	height = json["tile"].value("hright", 32);
	margin = json["tile"].value("margin", 0);
	spacing = json["tile"].value("spacing", 0);


	ntiles = json.value("ntiles", 0);
	columns = json.value("columns", 0);

	use_transparent = json.value("use transparent", false);
	transparent_color[0] = json["transparent color"].value("r", -1.f);
	transparent_color[1] = json["transparent color"].value("g", -1.f);
	transparent_color[2] = json["transparent color"].value("b", -1.f);
}

void r1Tileset::Unload()
{
	r1Texture* res = (r1Texture*)App->resources->Get(texture_uid);
	if (res != nullptr)
		res->Detach();
}

uint64_t r1Tileset::GetTextureUID() const
{
	return texture_uid;
}

void r1Tileset::OnInspector()
{
	ImGui::Text("Name: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.f, 0.6f, 0.6f, 1.f), name.c_str());

	auto image = App->resources->Get(texture_uid);
	ImGui::Text("Image:");
	ImGui::Indent();
	ImGui::Text("Path: ");
	ImGui::SameLine();
	if (image) {
		ImGui::TextColored(ImVec4(1.f, 0.6f, 0.6f, 1.f), image->path.c_str());
	}
	else {
		ImGui::TextColored(ImVec4(1.f, 0.6f, 0.6f, 1.f), "Unknown");
	}
	ImGui::Text("UID: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.f, 0.6f, 0.6f, 1.f), std::to_string(texture_uid).c_str());

	if (ImGui::Button("Edit")) {

	}

	ImGui::Unindent();

	ImGui::Text("Tile");
	ImGui::Indent();
	ImGui::InputInt("Width", &width);
	ImGui::InputInt("Height", &height);
	ImGui::InputInt("Margin", &margin);
	ImGui::InputInt("Spacing", &spacing);
	ImGui::Unindent();

	ImGui::NewLine();
	ImGui::InputInt("Columns", &columns);
	int rows = ntiles / columns;
	if (ImGui::InputInt("Rows", &rows))
		ntiles = rows * columns;

	ImGui::NewLine();
	ImGui::Checkbox("Use Transparent Color", &use_transparent);
	ImGui::ColorEdit3("Transparent Color", transparent_color);

	ImGui::NewLine();
	if (ImGui::Button("Save")) {

	}
}
