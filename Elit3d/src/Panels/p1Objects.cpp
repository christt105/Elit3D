#include "Panels/p1Objects.h"
#include "Core/Application.h"
#include "Modules/m1Objects.h"
#include "Objects/Object.h"
#include "Modules/m1GUI.h"
#include "Modules/m1Events.h"
#include "Panels/p1Inspector.h"
#include "Modules/m1Resources.h"
#include "Resources/r1Object.h"
#include "Panels/p1ObjectEditor.h"

#include "Tools/System/Logger.h"

p1Objects::p1Objects(bool start_active, bool appear_mainmenubar, bool can_close)
	: Panel("Objects", start_active, appear_mainmenubar, can_close, ICON_FA_SITEMAP)
{
}

p1Objects::~p1Objects()
{
}

void p1Objects::Update()
{
	float height = ImGui::GetContentRegionAvail().y;
	ImGui::BeginChild("Tree Objects", ImVec2(0.f, height*0.5f), true);
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
	ImGui::Separator();
	ImGui::Indent();
	auto obj = App->resources->GetVectorOf(Resource::Type::Object);
	for (auto i = obj.begin(); i != obj.end(); ++i) {
		ImGui::PushID(*i);
		if (ImGui::Selectable((*i)->name.c_str(), selected == *i)) {
			selected = (r1Object*)*i;
		}
		ImGui::PopID();
	}
	ImGui::Unindent();
	ImGui::EndChild();
}

uint64_t p1Objects::GetObjectSelected() const
{
	if (selected != nullptr)
		return selected->GetUID();
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
