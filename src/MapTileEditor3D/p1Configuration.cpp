#include "p1Configuration.h"

#include "Application.h"
#include "ExternalTools/ImGui/imgui.h"

p1Configuration::p1Configuration(bool start_enabled) : Panel("Configuration", start_enabled)
{
}

p1Configuration::~p1Configuration()
{
}

void p1Configuration::Update()
{
	ImGui::Text("Frames: %i", App->GetFrames());
	ImGui::Text("DT: %f", App->GetDt());
}
