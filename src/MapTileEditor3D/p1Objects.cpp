#include "p1Objects.h"
#include "Application.h"
#include "m1Objects.h"
#include "Object.h"

#include "Logger.h"

p1Objects::p1Objects(bool start_active) : Panel("Objects", start_active)
{
}

p1Objects::~p1Objects()
{
}

void p1Objects::Update()
{
	for (auto i = App->objects->objects.begin(); i != App->objects->objects.end(); ++i) {
		TreeNode(*i);
	}
}

void p1Objects::TreeNode(Object* obj)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	if (App->objects->selected == obj)
		flags |= ImGuiTreeNodeFlags_Selected;
	if (obj->children.empty())
		flags |= ImGuiTreeNodeFlags_Leaf;

	bool tree = ImGui::TreeNodeEx(obj->GetName(), flags);
	if (ImGui::IsItemClicked() && App->objects->selected != obj) {
		App->objects->selected = obj;
		LOG("Selected %i", obj);
	}
	if (tree) {
		for (auto i = obj->children.begin(); i != obj->children.end(); ++i) {
			TreeNode(*i);
		}
		ImGui::TreePop();
	}
}
