#include "p1Resources.h"

#include "Application.h"
#include "FileSystem.h"
#include "r1Texture.h"
#include "m1GUI.h"
#include "m1MapEditor.h"

#include "Profiler.h"

p1Project::p1Project(bool start_enabled, bool appear_mainmenubar, bool can_close)
	: Panel("Project", start_enabled, appear_mainmenubar, can_close, ICON_FA_FOLDER_OPEN)
{
	root = FileSystem::FileSystem::GetPtrFolder("Assets/");
	selected = root;
}

p1Project::~p1Project()
{
}

void p1Project::Start()
{
	r1Texture* t = nullptr;
	for (int i = (int)m1Resources::EResourceType::RESOURCE_MIN + 1; i < (int)m1Resources::EResourceType::RESOURCE_MAX; ++i) {
		t = (r1Texture*)App->resources->EGet((m1Resources::EResourceType)i);
		t->Attach();
		ids[(m1Resources::EResourceType)i] = t->GetBufferID();
	}
}

void p1Project::Update()
{
	PROFILE_FUNCTION();
	ImGui::Columns(2, "##resources", false);
	ImGui::SetColumnWidth(0, 200.f);
	ImGui::SliderFloat("##size", &size, 0.25f, 1.f, "");
	SideTreeFolder(root);
	ImGui::NextColumn();
	Folder* p = selected;
	std::vector<Folder*> folders;
	while (p->parent != nullptr) {
		folders.push_back(p);
		p = p->parent;
	}
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
	for (auto i = folders.rbegin(); i != folders.rend(); ++i) {
		ImGui::PushID(*i);
		if (ImGui::Button((*i)->name.c_str())) {
			selected = *i;
		}
		if ((i + 1) != folders.rend()) {
			ImGui::SameLine();
			ImGui::Text(ICON_FA_CHEVRON_RIGHT);
			ImGui::SameLine();
		}
		ImGui::PopID();
	}
	ImGui::PopStyleColor();

	if (size == 0.25f) { //List
		for (auto i = selected->folders.begin(); i != selected->folders.end(); ++i) {
			ImGui::PushID(*i);
			ImGui::AlignTextToFramePadding();
			if(ImGui::Selectable("##folders"))
				selected = *i;
			ImGui::SameLine();
			ImGui::Image((ImTextureID)ids[m1Resources::EResourceType::FOLDER], ImVec2(0.15f * 120.f, 0.15f * 100.f), ImVec2(0.f, 1.f), ImVec2(1.f, 0.f));
			ImGui::SameLine();
			ImGui::Text((*i)->name.c_str());
			ImGui::PopID();
		}

		for (auto i = selected->files.begin(); i != selected->files.end(); ++i) {
			std::string extension = FileSystem::GetFileExtension((*i).first.c_str());
			if (extension.compare("meta") != 0) {
				ImGui::PushID(&(*i).second);
				ImGui::AlignTextToFramePadding();
				if (ImGui::Selectable("##files")) { //TODO: make it bigger
					static std::string path;
					path = selected->full_path + (*i).first;
					App->gui->inspector->SetSelected((void*)&(path), GetInspectorType(extension));
				}
				ImGui::SameLine();
				ImGui::Image((ImTextureID)ids[GetEType(extension)], ImVec2(0.15f * 80.f, 0.15f * 100.6f), ImVec2(0.f, 1.f), ImVec2(1.f, 0.f));
				ImGui::SameLine();
				ImGui::Text(FileSystem::GetNameFile((*i).first.c_str()).c_str());
				ImGui::PopID();
			}
		}
	}
	else { //Grid
		for (auto i = selected->folders.begin(); i != selected->folders.end(); ++i) {
			ImGui::PushID(*i);
			ImGui::BeginGroup();
			if (ImGui::ImageButtonGL((ImTextureID)ids[m1Resources::EResourceType::FOLDER], ImVec2(size * 120.f, size * 100.f), 2)) {
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
				if (ImGui::ImageButtonGL((ImTextureID)ids[GetEType(extension)], ImVec2(size * 80.f, size * 100.6f), 2)) {
					static std::string path;
					path = selected->full_path + (*i).first;
					App->gui->inspector->SetSelected((void*)&(path), GetInspectorType(extension));
				}
				if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && ImGui::IsItemHovered()) {
					App->map_editor->LoadMap(App->resources->FindByPath((selected->full_path + (*i).first).c_str()));
				}
				ImGui::Text(FileSystem::GetNameFile((*i).first.c_str()).c_str());
				ImGui::EndGroup();
				if (ImGui::BeginPopupContextItem("##popupfile")) {
					if (ImGui::Selectable("Delete")) {
						App->resources->DeleteResource(App->resources->FindByPath((selected->full_path + (*i).first).c_str()));
					}
					ImGui::EndPopup();
				}
				ImGui::SameLine();
				ImGui::PopID();
			}
		}
	}
}

void p1Project::SideTreeFolder(const Folder* folder)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanFullWidth;
	if (folder->folders.empty())
		flags |= ImGuiTreeNodeFlags_Leaf;
	else
		flags |= ImGuiTreeNodeFlags_OpenOnArrow;
	if (folder == root)
		flags |= ImGuiTreeNodeFlags_DefaultOpen;

	bool open = ImGui::TreeNodeEx(folder->name.c_str(), flags);

	if (!ImGui::IsItemToggledOpen() && ImGui::IsItemClicked()) {
		selected = (Folder*)folder;
	}

	if (open)
		for (auto i = folder->folders.begin(); i != folder->folders.end(); ++i) {
			SideTreeFolder(*i);
		}

	if (open)
		ImGui::TreePop();

	/*
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
*/
}

m1Resources::EResourceType p1Project::GetEType(const std::string& extension) const
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

p1Inspector::SelectedType p1Project::GetInspectorType(const std::string& extension) const
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
