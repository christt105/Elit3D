#include "p1Resources.h"

#include <stack>

#include "Application.h"
#include "FileSystem.h"
#include "r1Texture.h"
#include "m1GUI.h"

p1Resources::p1Resources(bool start_enabled, bool appear_mainmenubar, bool can_close)
	: Panel("Resources", start_enabled, appear_mainmenubar, can_close, ICON_FA_FOLDER_OPEN)
{
	root = FileSystem::FileSystem::GetPtrFolder("Assets/");
	selected = root;
}

p1Resources::~p1Resources()
{
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
	ImGui::Columns(2, "##resources", false);
	ImGui::SetColumnWidth(0, 200.f);
	SideTreeFolder(root);
	ImGui::NextColumn();
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
	for (auto i = selected->folders.begin(); i != selected->folders.end(); ++i) {
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
			ImGui::PushID(&(*i).second);
			ImGui::BeginGroup();
			if (ImGui::ImageButton((ImTextureID)((r1Texture*)App->resources->EGet(GetEType(extension)))->GetBufferID(),
				ImVec2(40.f, 50.f), ImVec2(0.f, 1.f), ImVec2(1.f, 0.f), 2)) {
				static std::string path;
				path = selected->full_path + (*i).first;
				App->gui->inspector->SetSelected((void*)&(path), GetInspectorType(extension));
			}
			ImGui::Text(FileSystem::GetNameFile((*i).first.c_str()).c_str());
			ImGui::EndGroup();
			ImGui::SameLine();
			ImGui::PopID();
		}
	}
}

void p1Resources::SideTreeFolder(const Folder* folder)
{
	std::stack<Folder*> s;
	s.push((Folder*)folder);

	while (s.empty() == false) {
		Folder* f = s.top();
		s.pop();
		
		bool open = ImGui::TreeNodeEx(f->name.c_str(), (f->folders.empty()) ? ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnArrow);

		if (!ImGui::IsItemToggledOpen() && ImGui::IsItemClicked()) {
			selected = (Folder*)f;
		}

		if (open)
			for (auto i = f->folders.begin(); i != f->folders.end(); ++i) {
				s.push(*i);
			}

		if (open)
			ImGui::TreePop(); //TODO: modify to use tree indent
	}
}

m1Resources::EResourceType p1Resources::GetEType(const std::string& extension) const
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

p1Inspector::SelectedType p1Resources::GetInspectorType(const std::string& extension) const
{
	if (extension.compare("png") == 0)
		return p1Inspector::SelectedType::PNG;
	if (extension.compare("scene") == 0)
		return p1Inspector::SelectedType::MAP;
	if (extension.compare("fbx") == 0)
		return p1Inspector::SelectedType::FBX;
	if (extension.compare("tileset") == 0)
		return p1Inspector::SelectedType::TILESET;


	return p1Inspector::SelectedType::NONE;
}
