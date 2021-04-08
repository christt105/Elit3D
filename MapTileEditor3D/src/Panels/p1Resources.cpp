#include "Panels/p1Resources.h"

#include <stack>

#include "Core/Application.h"
#include "Tools/FileSystem.h"
#include "Resources/r1Texture.h"
#include "Modules/m1GUI.h"
#include "Modules/m1MapEditor.h"

#include "Panels/p1Inspector.h"

#include "Resources/r1Model.h"
#include "Resources/r1Map.h"
#include "Tools/Map/MapLayer.h"
#include "Tools/Map/MapLayerObject.h"

#include "Tools/System/OSUtils.h"

#include "Tools/System/Profiler.h"

#include "ExternalTools/mmgr/mmgr.h"

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
		DragDropTargetFolder((*i)->full_path);
		if ((i + 1) != folders.rend()) {
			ImGui::SameLine();
			ImGui::Text(ICON_FA_CHEVRON_RIGHT);
			ImGui::SameLine();
		}
		ImGui::PopID();
	}
	ImGui::PopStyleColor();

	for (auto i = selected->folders.begin(); i != selected->folders.end(); ++i) {
		ImGui::PushID(*i);
		if (size <= 0.25f) { //List
			ImGui::AlignTextToFramePadding();
			ImGui::BeginGroup();
			if (ImGui::Selectable("##folders")) {
				selected = *i;
				ImGui::PopID();
				return;
			}
			ImGui::SameLine();
			ImGui::Image((ImTextureID)ids[m1Resources::EResourceType::FOLDER], ImVec2(0.15f * 120.f, 0.15f * 100.f), ImVec2(0.f, 1.f), ImVec2(1.f, 0.f));
			ImGui::SameLine();
			ImGui::Text((*i)->name.c_str());
			ImGui::EndGroup();
		}
		else { //Grid
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
		}
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
			ImGui::SetDragDropPayload("DND_FOLDER_PROJECT", (*i), sizeof(int));
			ImGui::Text("Not implemented");// (*i)->full_path.c_str());
			ImGui::EndDragDropSource();
		}
		DragDropTargetFolder((*i)->full_path);
		if (ImGui::BeginPopupContextItem("##popupfile")) {
			if (ImGui::Selectable("Create Folder")) {
				if (FileSystem::Exists(((*i)->full_path + "New Folder").c_str())) {
					int index = 1;
					while (FileSystem::Exists(((*i)->full_path + "New Folder (" + std::to_string(index) + ")").c_str())) {
						++index;
					}
					FileSystem::CreateFolder(((*i)->full_path + "New Folder (" + std::to_string(index) + ")").c_str());
				}
				else {
					FileSystem::CreateFolder(((*i)->full_path + "New Folder").c_str());
				}
			}
			if (ImGui::Selectable("Open")) {
				OSUtils::Open(FileSystem::GetFullPath((*i)->full_path.c_str()).c_str());
			}
			if (ImGui::Selectable("Copy Path")) {
				OSUtils::SetClipboardText((*i)->full_path.c_str());
			}
			if (ImGui::Selectable("Rename")) {
			}
			if (ImGui::Selectable("Show in Explorer")) {
				OSUtils::OpenAndSelect(FileSystem::GetFullPath((*i)->full_path.c_str()).c_str());
			}
			if (ImGui::Selectable("Delete")) {
				App->resources->PauseFileWatcher(true);
				std::stack<Folder*> stack;
				std::stack<Folder*> stkfolders;
				stack.push(*i);
				stkfolders.push(*i);
				while (!stkfolders.empty()) {
					Folder* folder = stkfolders.top();
					stkfolders.pop();
					for (auto it = folder->folders.begin(); it != folder->folders.end(); ++it) {
						stack.push(*it);
						stkfolders.push(*it);
					}
				}
				while (!stack.empty()) {
					Folder* f = stack.top();
					stack.pop();
					auto itfile = f->files.begin();
					while (itfile != f->files.end()) {
						if (FileSystem::GetFileExtension((*itfile).first.c_str()).compare("meta") != 0) {
							uint64_t uid = App->resources->FindByPath((f->full_path + (*itfile).first).c_str());
							if (uid == 0) {
								FileSystem::fDeleteFile((f->full_path + (*itfile).first).c_str());
							}
							else {
								App->resources->DeleteResource(uid);
								itfile = f->files.begin();
							}
						}
						++itfile;
					}
					FileSystem::DeleteFolder(f->full_path.c_str());
				}
				App->resources->PauseFileWatcher(false);
				ImGui::EndPopup();
				ImGui::PopID();
				break;
			}
			ImGui::EndPopup();
		}
		ImGui::PopID();
	}

	for (auto i = selected->files.begin(); i != selected->files.end(); ++i) {
		std::string extension = FileSystem::GetFileExtension((*i).first.c_str());
		if (extension.compare("meta") != 0) {
			ImGui::PushID(&(*i).second);
			if (size <= 0.25f) { // List
				ImGui::AlignTextToFramePadding();
				ImGui::BeginGroup();
				if (ImGui::Selectable("##files")) { //TODO: make it bigger
					static std::string path;
					path = selected->full_path + (*i).first;
					App->gui->inspector->SetSelected((void*)&(path), p1Inspector::SelectedType::RESOURCE);
				}
				ImGui::SameLine();
				ImGui::Image((ImTextureID)ids[GetEType(extension)], ImVec2(0.15f * 80.f, 0.15f * 100.6f), ImVec2(0.f, 1.f), ImVec2(1.f, 0.f));
				ImGui::SameLine();
				ImGui::Text(FileSystem::GetNameFile((*i).first.c_str()).c_str());
				ImGui::EndGroup();
			}
			else { // Grid
				ImGui::BeginGroup();
				if (ImGui::ImageButtonGL((ImTextureID)ids[GetEType(extension)], ImVec2(size * 80.f, size * 100.6f), 2)) {
					static std::string path;
					path = FileSystem::GetCanonical((selected->full_path + (*i).first).c_str());
					App->gui->inspector->SetSelected((void*)&(path), p1Inspector::SelectedType::RESOURCE);
				}
				if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && ImGui::IsItemHovered()) {
					std::string ext = FileSystem::GetFileExtension((*i).first.c_str());

					if (ext == "fbx" || ext == "obj") { //TODO ugly place to put that code here
						auto model = (r1Model*)App->resources->Get(App->resources->FindByPath((selected->full_path + "/" + (*i).first).c_str()));
						model->Attach();
						auto map = App->map_editor->GetObjectLayer(true, true);
						model->CreateObject(map->root);
					}
					else if (ext == "scene") {
						App->map_editor->LoadMap(App->resources->FindByPath((selected->full_path + "/" + (*i).first).c_str()));
					}
				}
				ImGui::Text(FileSystem::GetNameFile((*i).first.c_str()).c_str());
				ImGui::EndGroup();
				ImGui::SameLine();
			}
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
				payload = selected->full_path + (*i).first;
				ImGui::SetDragDropPayload("DND_FILE_PROJECT", &(*i).first, sizeof(int));
				ImGui::Text((*i).first.c_str());
				ImGui::EndDragDropSource();
			}
			if (ImGui::BeginPopupContextItem("##popupfile")) {
				if (ImGui::Selectable("Open")) {
					OSUtils::Open(FileSystem::GetFullPath((selected->full_path + (*i).first).c_str()).c_str());
				}
				if (ImGui::Selectable("Copy Path")) {
					OSUtils::SetClipboardText((selected->full_path + (*i).first).c_str());
				}
				if (ImGui::Selectable("Rename")) {
				}
				if (ImGui::Selectable("Show in Explorer")) {
					OSUtils::OpenAndSelect(FileSystem::GetFullPath((selected->full_path + (*i).first).c_str()).c_str());
				}
				if (ImGui::Selectable("Delete")) {
					App->resources->DeleteResource(App->resources->FindByPath((selected->full_path + (*i).first).c_str()));
				}
				ImGui::EndPopup();
			}
			ImGui::PopID();
		}
	}

	if (ImGui::BeginPopupContextWindow("##popupwindow", 1, false)) {
		if (ImGui::Selectable("Create Folder")) {
			if (FileSystem::Exists((selected->full_path + "New Folder").c_str())) {
				int index = 1;
				while (FileSystem::Exists((selected->full_path + "New Folder (" + std::to_string(index) + ")").c_str())) {
					++index;
				}
				FileSystem::CreateFolder((selected->full_path + "New Folder (" + std::to_string(index) + ")").c_str());
			}
			else {
				FileSystem::CreateFolder((selected->full_path + "New Folder").c_str());
			}
		}

		ImGui::EndPopup();
	}
}

void p1Project::DragDropTargetFolder(const std::string& folder)
{
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* impayload = ImGui::AcceptDragDropPayload("DND_FOLDER_PROJECT")) {
			IM_ASSERT(impayload->DataSize == sizeof(int));

			Folder* payload = (Folder*)impayload->Data;
			App->resources->PauseFileWatcher(true);
			//FileSystem::MoveTo(payload->full_path.c_str(), folder.c_str());
			App->resources->PauseFileWatcher(false);
		}
		if (const ImGuiPayload* impayload = ImGui::AcceptDragDropPayload("DND_FILE_PROJECT"))
		{
			IM_ASSERT(impayload->DataSize == sizeof(int));
			App->resources->PauseFileWatcher(true);
			FileSystem::MoveTo(payload.c_str(), (folder + FileSystem::GetNameFile(payload.c_str(), true)).c_str());
			FileSystem::MoveTo((payload + ".meta").c_str(), (folder + FileSystem::GetNameFile(payload.c_str(), true) + ".meta").c_str());
			auto r = App->resources->FindGet(payload.c_str(), false);
			if (r != nullptr)
				r->path.assign((folder + FileSystem::GetNameFile(payload.c_str(), true)).c_str());
			std::this_thread::sleep_for(std::chrono::milliseconds(10)); // TODO: Pause filewatcher stops the loop, and reanude reimport root
			App->resources->PauseFileWatcher(false);
		}
		ImGui::EndDragDropTarget();
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

	DragDropTargetFolder(folder->full_path);
	if (open) {
		for (auto i = folder->folders.begin(); i != folder->folders.end(); ++i) {
			SideTreeFolder(*i);
		}
	}

	if (open)
		ImGui::TreePop();
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
