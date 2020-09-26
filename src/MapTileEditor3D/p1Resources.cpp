#include "p1Resources.h"

#include "Application.h"
#include "FileSystem.h"
#include "r1Texture.h"

p1Resources::p1Resources(bool start_enabled, bool appear_mainmenubar, bool can_close)
	: Panel("Resources", start_enabled, appear_mainmenubar, can_close, ICON_FA_FOLDER_OPEN)
{
	root = FileSystem::_GetFilesRecursive("Assets/");
	selected = root;
}

p1Resources::~p1Resources()
{
	delete root;
}

void p1Resources::Start()
{
	App->resources->EGet(m1Resources::EResourceType::FOLDER_BACK)->Attach();
	App->resources->EGet(m1Resources::EResourceType::FOLDER)->Attach();
	App->resources->EGet(m1Resources::EResourceType::PNG)->Attach();
	App->resources->EGet(m1Resources::EResourceType::FBX)->Attach();
	App->resources->EGet(m1Resources::EResourceType::TILESET)->Attach();
	App->resources->EGet(m1Resources::EResourceType::MAP)->Attach();
	App->resources->EGet(m1Resources::EResourceType::UNKNOWN)->Attach();
}

void p1Resources::Update()
{
	if (selected != root) {
		ImGui::BeginGroup();
		if (ImGui::ImageButton((ImTextureID)((r1Texture*)App->resources->EGet(m1Resources::EResourceType::FOLDER_BACK))->GetBufferID(),
			ImVec2(40.f, 50.f), ImVec2(0.f, 1.f), ImVec2(1.f, 0.f), 2)) {
			selected = selected->parent;
			ImGui::EndGroup();
			return;
		}
		ImGui::Text("Back");
		ImGui::EndGroup();
		ImGui::SameLine();
	}
	for (auto i = selected->_folders.begin(); i != selected->_folders.end(); ++i) {
		ImGui::PushID(*i);
		ImGui::BeginGroup();
		if (ImGui::ImageButton((ImTextureID)((r1Texture*)App->resources->EGet(m1Resources::EResourceType::FOLDER))->GetBufferID(),
			ImVec2(40.f, 50.f), ImVec2(0.f, 1.f), ImVec2(1.f, 0.f), 2)) {
			selected = *i;
			ImGui::EndGroup();
			ImGui::PopID();
			return;
		}
		ImGui::Text((*i)->name.c_str());
		ImGui::EndGroup();
		ImGui::SameLine();
		ImGui::PopID();
	}

	for (auto i = selected->files.begin(); i != selected->files.end(); ++i) {
		std::string extension = FileSystem::GetFileExtension((*i).first.c_str());
		if (extension.compare("meta") != 0) {
			ImGui::PushID((*i).second);
			ImGui::BeginGroup();
			if (ImGui::ImageButton((ImTextureID)((r1Texture*)App->resources->EGet(GetType(extension)))->GetBufferID(),
				ImVec2(40.f, 50.f), ImVec2(0.f, 1.f), ImVec2(1.f, 0.f), 2)) {
			}
			ImGui::Text((*i).first.c_str());
			ImGui::EndGroup();
			ImGui::SameLine();
			ImGui::PopID();
		}
	}
}

m1Resources::EResourceType p1Resources::GetType(const std::string& extension) const
{
	if (extension.compare("png") == 0)
		return m1Resources::EResourceType::PNG;
	if (extension.compare("scene") == 0)
		return m1Resources::EResourceType::MAP;
	if (extension.compare("fbx") == 0)
		return m1Resources::EResourceType::FBX;
	if (extension.compare("tileset") == 0)
		return m1Resources::EResourceType::TILESET;


	return m1Resources::EResourceType::UNKNOWN;
}
