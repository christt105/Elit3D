#include "p1Configuration.h"

#include "Application.h"
#include "m1Camera3D.h"
#include "m1Render3D.h"
#include "m1Window.h"
#include "m1Scene.h"

#include "GL/glew.h"

p1Configuration::p1Configuration(bool start_enabled) : Panel("Configuration", start_enabled, ICON_FA_WRENCH)
{
}

p1Configuration::~p1Configuration()
{
}

void p1Configuration::Update()
{
	if (ImGui::CollapsingHeader("Window")) {
		if (ImGui::DragInt("Width", &App->window->width))
			App->window->UpdateWindowSize();
		if(ImGui::DragInt("Height", &App->window->height))
			App->window->UpdateWindowSize();
	}

	if (ImGui::CollapsingHeader("Render")) {
		ImGui::Text("FrameRate: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 0.6470f, 0.f, 1.f), "%u", App->GetFrameRate());
		ImGui::Checkbox("Draw Grid", &App->scene->draw_grid);
		static bool wired = false;
		if(ImGui::Checkbox("Wired Mode", &wired))
			if (wired) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			else
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
		ImGui::ColorEdit3("Background Color", App->render->background_color);
	}

	if (ImGui::CollapsingHeader("Camera Control")) {
		ImGui::Text("Camera Position: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 0.6470f, 0.f, 1.f), "%s", App->camera->frustum.pos.ToString().c_str());
		ImGui::SliderFloat("Pan Speed", &App->camera->pan_speed, 0.1f, 2.f);
		ImGui::SliderFloat("Orbit Speed", &App->camera->orbit_speed, 0.01f, 0.5f);
		ImGui::SliderFloat("Zoom Speed", &App->camera->zoom_speed, 1.f, 100.f);
		ImGui::SliderFloat("Movement Speed", &App->camera->mov_speed, 1.f, 50.f);
		if (ImGui::SliderFloat("FOV", &App->camera->FOV, 30.f, 120.f))
			App->camera->SetFov();
	}
}
