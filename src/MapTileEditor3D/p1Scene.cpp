#include "p1Scene.h"

#include "Application.h"
#include "m1Window.h"
#include "m1Camera3D.h"
#include "m1Render3D.h"

#include "Viewport.h"

#include "Logger.h"

#include "ExternalTools/ImGui/imgui_internal.h"

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
	if (ImGui::BeginMenuBar()) {
		if (App->camera->frustum.type == FrustumType::PerspectiveFrustum) {
			if (ImGui::Button("2D")) {
				App->camera->frustum.front = -float3::unitY;
				App->camera->frustum.up = float3::unitX;
				App->camera->frustum.type = FrustumType::OrthographicFrustum;
				/*App->camera->frustum.orthographicWidth = window_size.x/25;
				App->camera->frustum.orthographicHeight = window_size.y/25;*/
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

	ImGui::PushClipRect(ImGui::GetWindowPos(), ImGui::GetWindowPos() + ImGui::GetWindowSize(), false);
	ImGui::SetCursorScreenPos(ImGui::GetWindowPos() + ImVec2(0, ImGui::GetCurrentWindow()->TitleBarHeight() + ImGui::GetCurrentWindow()->MenuBarHeight()));

	ImVec2 window_size = ImGui::GetContentRegionAvail() + ImVec2(16, 16);
	viewport->UpdateSize(window_size.x, window_size.y);
	viewport->Update();

	viewport->Blit();

	ImGui::Image((ImTextureID)viewport->GetTexture(), window_size, ImVec2(0,0), ImVec2(1,-1));

	ImGui::PopClipRect();

	if (App->camera->frustum.type == FrustumType::PerspectiveFrustum) // TODO: extract all related with frustum to a viewport class in order to have more viewports with opengl objects
		App->camera->frustum.horizontalFov = 2.f * atanf(tanf(App->camera->frustum.verticalFov * 0.5f) * window_size.x / window_size.y);
}
