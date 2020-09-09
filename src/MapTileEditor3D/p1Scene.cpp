#include "p1Scene.h"

#include "Application.h"
#include "m1Window.h"
#include "m1Camera3D.h"
#include "m1Render3D.h"
#include "m1MapEditor.h"

#include "r1Map.h"

#include "Viewport.h"

#include "Logger.h"

#include "ExternalTools/ImGui/imgui_internal.h"




//temp 
#include "ExternalTools/MathGeoLib/include/Math/MathFunc.h"

p1Scene::p1Scene(bool start_enabled): Panel("Scene", start_enabled, ICON_FA_CUBES)
{
	flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
}

void p1Scene::Start()
{
	viewport = App->render->CreateViewport();
}

p1Scene::~p1Scene()
{
}

void p1Scene::Update()
{
	MenuBar();

	if (App->map_editor->ValidMap()) {

		ImGui::PushClipRect(ImGui::GetWindowPos(), ImGui::GetWindowPos() + ImGui::GetWindowSize(), false);
		ImGui::SetCursorScreenPos(ImGui::GetWindowPos() + ImVec2(0, ImGui::GetCurrentWindow()->TitleBarHeight() + ImGui::GetCurrentWindow()->MenuBarHeight()));

		ImVec2 window_size = ImGui::GetContentRegionAvail() + ImVec2(16, 16);
		viewport->UpdateSize(window_size.x, window_size.y);
		App->camera->frustum.verticalFov = DegToRad(60.f);
		App->camera->frustum.horizontalFov = 2.f * atanf(tanf(App->camera->frustum.verticalFov * 0.5f) * (window_size.x / window_size.y));
		viewport->Update();

		viewport->Blit();

		ImGui::Image((ImTextureID)viewport->GetTexture(), window_size, ImVec2(0, 0), ImVec2(1, -1));

		ImGui::PopClipRect();
	}
	else {
		ImGui::SetCursorScreenPos(ImGui::GetWindowPos() + ImGui::GetWindowSize() * 0.5f - ImVec2(50, 10));
		if (ImGui::Button("Create Map", ImVec2(100, 20))) {
			ImGui::OpenPopup("Create Map");
		}
		ImGui::SetNextWindowSize(ImVec2(350, 250), ImGuiCond_Always);
		ImGui::SetNextWindowPos(ImVec2((float)App->window->GetWidth() * 0.5f - 350.f * 0.5f, (float)App->window->GetHeight() * 0.5f - 250.f * 0.5f));
		if (ImGui::BeginPopupModal("Create Map", 0, ImGuiWindowFlags_::ImGuiWindowFlags_NoMove)) {
			static char name[30] = { ' ' };
			ImGui::InputText("Name:", name, 30);

			ImGui::Text("Size:");
			static int size[2] = { 10, 10 };
			ImGui::InputInt("Width", &size[0]);
			ImGui::InputInt("Height", &size[1]);

			ImGui::NewLine();

			if (ImGui::Button("Save")) {
				r1Map::CreateNewMap(size[0], size[1]);
				
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel")) {
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	/*if (App->camera->frustum.type == FrustumType::PerspectiveFrustum) // TODO: extract all related with frustum to a viewport class in order to have more viewports with opengl objects
		App->camera->frustum.horizontalFov = 2.f * atanf(tanf(App->camera->frustum.verticalFov * 0.5f) * window_size.x / window_size.y);*/
}

void p1Scene::MenuBar()
{
	if (ImGui::BeginMenuBar()) {
		if (App->camera->frustum.type == FrustumType::PerspectiveFrustum) {
			if (ImGui::Button("2D")) {
				App->camera->frustum.front = -float3::unitY;
				App->camera->frustum.up = float3::unitX;
				App->camera->frustum.type = FrustumType::OrthographicFrustum;
				App->camera->frustum.orthographicHeight = 10;
				App->camera->frustum.orthographicWidth = 20 / App->camera->frustum.AspectRatio();
			}
		}
		else {
			if (ImGui::Button("3D")) {
				App->camera->frustum.type = FrustumType::PerspectiveFrustum;
				App->camera->SetFov();
			}
		}

		ImGui::EndMenuBar();
	}
}
