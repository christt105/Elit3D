#include "p1Scene.h"

#include <GL/glew.h>

#include "Application.h"
#include "m1Window.h"
#include "m1Camera3D.h"

#include "ExternalTools/ImGui/imgui_internal.h"

p1Scene::p1Scene(bool start_enabled): Panel("Scene", start_enabled, ICON_FA_CUBES)
{
	flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
}

void p1Scene::InitFrameBuffer()
{
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// generate texture
	glGenTextures(1, &tex_buf);
	glBindTexture(GL_TEXTURE_2D, tex_buf);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, App->window->GetWidth(), App->window->GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_buf, 0);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, App->window->GetWidth(), App->window->GetHeight());
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
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
				App->camera->frustum.orthographicWidth = window_size.x/25;
				App->camera->frustum.orthographicHeight = window_size.y/25;
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
	window_size = ImGui::GetContentRegionAvail() + ImVec2(16, 16);
	ImGui::SetCursorScreenPos(ImGui::GetWindowPos() + ImVec2(0, ImGui::GetCurrentWindow()->TitleBarHeight() + ImGui::GetCurrentWindow()->MenuBarHeight()));
	ImGui::Image((ImTextureID)tex_buf, window_size, ImVec2(0,0), ImVec2(1,-1));
	ImGui::PopClipRect();

	if (App->camera->frustum.type == FrustumType::PerspectiveFrustum) // TODO: extract all related with frustum to a viewport class in order to have more viewports with opengl objects
		App->camera->frustum.horizontalFov = 2.f * atanf(tanf(App->camera->frustum.verticalFov * 0.5f) * window_size.x / window_size.y);
}

void p1Scene::SelectFrameBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
}

void p1Scene::DeselectFrameBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
	glClearColor(.1f, .1f, .1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}
