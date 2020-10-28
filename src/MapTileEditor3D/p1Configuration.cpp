#include "p1Configuration.h"

#include "Application.h"
#include "m1Camera3D.h"
#include "m1Render3D.h"
#include "m1Window.h"
#include "m1Scene.h"
#include "m1Events.h"

#include "m1GUI.h"
#include "p1Scene.h"
#include "Viewport.h"

#include "OpenGLHelper.h"

p1Configuration::p1Configuration(bool start_enabled, bool appear_mainmenubar, bool can_close)
	: Panel("Configuration", start_enabled, appear_mainmenubar, can_close, ICON_FA_WRENCH)
{
	flags = ImGuiWindowFlags_MenuBar;
}

p1Configuration::~p1Configuration()
{
}

void p1Configuration::Update()
{
	if (ImGui::BeginMenuBar()) {
		if (ImGui::Button("Save")) {
			App->events->AddEvent(new m1Events::Event(m1Events::Event::Type::SAVE_CONFIGURATION));
		}

		if (ImGui::Button("Load")) {
			App->events->AddEvent(new m1Events::Event(m1Events::Event::Type::LOAD_CONFIGURATION));
		}

		if (ImGui::Button("Set Default")) {
			App->events->AddEvent(new m1Events::Event(m1Events::Event::Type::LOAD_DEFAULT_CONFIGURATION));
		}

		ImGui::EndMenuBar();
	}
	if (ImGui::CollapsingHeader("Render", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Text("FrameRate: "); ImGui::SameLine(); ImGui::TextColored(ORANGE, "%.0f", ImGui::GetIO().Framerate);
		ImGui::Checkbox("Draw Grid", &App->scene->draw_grid);
		static bool wired = false;
		if (ImGui::Checkbox("Wired Mode", &wired))
			oglh::PolygonMode(wired);

		ImGui::ColorEdit3("Background Color", App->gui->scene->viewport->color);
	}

	if (ImGui::CollapsingHeader("Camera Control", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Text("Camera Position: "); ImGui::SameLine(); ImGui::TextColored(ORANGE, "%s", App->camera->frustum.pos.ToString().c_str());
		ImGui::SliderFloat("Pan Speed", &App->camera->pan_speed, 0.1f, 2.f);
		ImGui::SliderFloat("Orbit Speed", &App->camera->orbit_speed, 0.01f, 0.5f);
		ImGui::SliderFloat("Zoom Speed", &App->camera->zoom_speed, 1.f, 100.f);
		ImGui::SliderFloat("Movement Speed", &App->camera->mov_speed, 1.f, 50.f);
		ImGui::SliderFloat("Turbo Speed", &App->camera->turbo_speed, 1.f, 50.f);
		if (ImGui::SliderFloat("FOV", &App->camera->FOV, 30.f, 120.f))
			App->camera->SetFov();
	}

	if (ImGui::CollapsingHeader("System Info", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Text("OS: "); ImGui::SameLine(); ImGui::TextColored(ORANGE, "%s", App->sys_info.platform_fullname.c_str());
		ImGui::Text("CPU: "); ImGui::SameLine(); ImGui::TextColored(ORANGE, "%s", App->sys_info.cpu_name.c_str());
		ImGui::Text("RAM: "); ImGui::SameLine(); ImGui::TextColored(ORANGE, "%iMB", App->sys_info.ram_mb);
		ImGui::Text("GPU: "); ImGui::SameLine(); ImGui::TextColored(ORANGE, "%s\n%s",App->sys_info.vendor.c_str(), App->sys_info.model.c_str());
	}
}
