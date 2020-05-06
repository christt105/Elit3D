#include "p1Configuration.h"

#include "Application.h"
#include "m1Camera3D.h"
#include "ExternalTools/ImGui/imgui.h"

p1Configuration::p1Configuration(bool start_enabled) : Panel("Configuration", start_enabled)
{
}

p1Configuration::~p1Configuration()
{
}

void p1Configuration::Update()
{
	if (ImGui::CollapsingHeader("Camera Control")) {
		ImGui::SliderFloat("Pan Speed", &App->camera->pan_speed, 0.1f, 2.f);
		ImGui::SliderFloat("Orbit Speed", &App->camera->orbit_speed, 0.01f, 0.5f);
		ImGui::SliderFloat("Zoom Speed", &App->camera->zoom_speed, 1.f, 100.f);
		if (ImGui::SliderFloat("FOV", &App->camera->FOV, 30.f, 120.f))
			App->camera->SetFov();
	}
}
