#include "p1Resources.h"

#include "Application.h"
#include "FileSystem.h"

p1Resources::p1Resources(bool start_enabled) : Panel("Resources", start_enabled, ICON_FA_FOLDER_OPEN)
{
	root = App->file_system->GetFilesRecursive("Assets/");
}

p1Resources::~p1Resources()
{
}

void p1Resources::Update()
{
	DisplayFolder(root, true);
}

void p1Resources::DisplayFolder(const Folder& folder, bool default_open) {
	bool opened = ImGui::TreeNodeEx(&folder, (default_open) ? ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_None, folder.name.c_str());

	if (opened) {
		for (auto i = folder.folders.begin(); i != folder.folders.end(); ++i) {
			DisplayFolder(*i, false);
		}
		for (auto i = folder.files.begin(); i != folder.files.end(); ++i) {
			if ((*i).first.compare((*i).first.length() - 5, (*i).first.length(), ".meta") != 0)
				if (ImGui::TreeNodeEx(&*i, ImGuiTreeNodeFlags_Leaf, (*i).first.c_str()))
					ImGui::TreePop();
		}

		ImGui::TreePop();
	}
}