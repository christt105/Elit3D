#include "p1DebugResources.h"
#include "Application.h"
#include "m1Resources.h"

p1DebugResources::p1DebugResources(bool start_enabled, bool appear_mainmenubar, bool can_close)
	: Panel("Debug Resources", start_enabled, appear_mainmenubar, can_close, ICON_FA_FILE)
{
}

p1DebugResources::~p1DebugResources()
{
}

void p1DebugResources::Update()
{
	auto resources = App->resources->GetResources();
	ImGui::Columns(6, "resdata");
	ImGui::Separator();
	ImGui::Text("Name"); ImGui::NextColumn();
	ImGui::Text("UID"); ImGui::NextColumn();
	ImGui::Text("Type"); ImGui::NextColumn();
	ImGui::Text("Extension"); ImGui::NextColumn();
	ImGui::Text("References"); ImGui::NextColumn();
	ImGui::Text("Path"); ImGui::NextColumn();
	ImGui::Separator();
	for (auto i = resources.begin(); i != resources.end(); ++i) {
		ImGui::Text((*i).second->name.c_str()); ImGui::NextColumn();
		ImGui::Text(std::to_string((*i).first).c_str()); ImGui::NextColumn();
		ImGui::Text((*i).second->GetStrType().c_str()); ImGui::NextColumn();
		ImGui::Text((*i).second->extension.c_str()); ImGui::NextColumn();
		ImGui::Text(std::to_string((*i).second->references).c_str()); ImGui::NextColumn();
		ImGui::Text((*i).second->path.c_str()); ImGui::NextColumn();
		ImGui::Separator();
	}
}
