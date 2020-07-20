#include "p1DebugResources.h"
#include "Application.h"
#include "m1Resources.h"

p1DebugResources::p1DebugResources() : Panel("Debug Resources", false, ICON_FA_FILE)
{
}

p1DebugResources::~p1DebugResources()
{
}

void p1DebugResources::Update()
{
	auto resources = App->resources->GetResources();
	ImGui::Columns(4, "resdata");
	ImGui::Separator();
	ImGui::Text("Name"); ImGui::NextColumn();
	ImGui::Text("UID"); ImGui::NextColumn();
	ImGui::Text("Type"); ImGui::NextColumn();
	ImGui::Text("Extension"); ImGui::NextColumn();
	ImGui::Separator();
	for (auto i = resources.begin(); i != resources.end(); ++i) {
		ImGui::Text((*i).second->name.c_str()); ImGui::NextColumn();
		ImGui::Text(std::to_string((*i).first).c_str()); ImGui::NextColumn();
		ImGui::Text((*i).second->GetStrType().c_str()); ImGui::NextColumn();
		ImGui::Text((*i).second->extension.c_str()); ImGui::NextColumn();
		ImGui::Separator();
	}
}
