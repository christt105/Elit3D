#include "p1About.h"

#include "Application.h"
#include "ExternalTools/ImGui/imgui.h"

p1About::p1About(bool start_enabled) : Panel("About", start_enabled)
{
}

p1About::~p1About()
{
}

void p1About::Update()
{
	ImGui::TextWrapped("%s v.%s\n\n", App->GetName(), App->GetVersion()); 
	ImGui::Text("Made by "); ImGui::SameLine();
	if (ImGui::Button("@christt105")) {
		App->ExecuteURL("https://github.com/christt105");
	}
	ImGui::TextWrapped("Licensed under MIT LICENSE, see LICENSE file.\n\nThis is a program made for my final degree project.\n\nYou can find more information in github repository and in the wiki page.");
	ImGui::Spacing();
	if (ImGui::Button("Repository")) {
		App->ExecuteURL("https://github.com/christt105/MapTileEditor-3D");
	}
	ImGui::SameLine();
	if (ImGui::Button("Wiki")) {
		App->ExecuteURL("https://github.com/christt105/MapTileEditor-3D/wiki");
	}
}
