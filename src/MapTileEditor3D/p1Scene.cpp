#include "p1Scene.h"

#include <GL/glew.h>

#include "Application.h"
#include "m1Window.h"
#include "m1Camera3D.h"

#include "Logger.h"

#include "ExternalTools/ImGui/imgui_internal.h"

p1Scene::p1Scene(bool start_enabled): Panel("Scene", start_enabled, ICON_FA_CUBES)
{
	flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
}

void p1Scene::InitFrameBuffer()
{
	int smaa = 8;
	glGenFramebuffers(1, &ID[FBO_MS]);
	glBindFramebuffer(GL_FRAMEBUFFER, ID[FBO_MS]);

	// generate texture
	glGenTextures(1, &ID[TEXTURE_MS]);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, ID[TEXTURE_MS]);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, smaa, GL_RGB, App->window->GetWidth(), App->window->GetHeight(), GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, ID[TEXTURE_MS], 0);

	glGenRenderbuffers(1, &ID[RBO_MS]);
	glBindRenderbuffer(GL_RENDERBUFFER, ID[RBO_MS]);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, smaa, GL_DEPTH24_STENCIL8, App->window->GetWidth(), App->window->GetHeight());
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, ID[RBO_MS]);

	//========================================================================

	glGenFramebuffers(1, &ID[FBO]);
	glBindFramebuffer(GL_FRAMEBUFFER, ID[FBO]);

	// generate texture
	glGenTextures(1, &ID[TEXTURE]);
	glBindTexture(GL_TEXTURE_2D, ID[TEXTURE]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, App->window->GetWidth(), App->window->GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ID[TEXTURE], 0);

	glGenRenderbuffers(1, &ID[RBO]);
	glBindRenderbuffer(GL_RENDERBUFFER, ID[RBO]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, App->window->GetWidth(), App->window->GetHeight());
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, ID[RBO]);

	glBindFramebuffer(GL_FRAMEBUFFER, ID[FBO_MS]);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
	glEnable(GL_DEPTH_TEST);
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
	ImGui::SetCursorScreenPos(ImGui::GetWindowPos() + ImVec2(0, ImGui::GetCurrentWindow()->TitleBarHeight() + ImGui::GetCurrentWindow()->MenuBarHeight()));
	window_size = ImGui::GetContentRegionAvail() + ImVec2(16, 16);

	// =========================================
	//=== Update tex
	glBindFramebuffer(GL_FRAMEBUFFER, ID[FBO_MS]);

	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, ID[TEXTURE_MS]);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, window_size.x, window_size.y, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, ID[TEXTURE_MS], 0);

	glBindRenderbuffer(GL_RENDERBUFFER, ID[RBO_MS]);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, window_size.x, window_size.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, ID[RBO_MS]);

	//========================================================================

	glBindFramebuffer(GL_FRAMEBUFFER, ID[FBO]);

	// generate texture
	glBindTexture(GL_TEXTURE_2D, ID[TEXTURE]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_size.x, window_size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ID[TEXTURE], 0);

	glBindRenderbuffer(GL_RENDERBUFFER, ID[RBO]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, window_size.x, window_size.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, ID[RBO]);

	glBindFramebuffer(GL_FRAMEBUFFER, ID[FBO_MS]);
	//======================
	glBindFramebuffer(GL_READ_FRAMEBUFFER, ID[FBO_MS]);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, ID[FBO]);
	glBlitFramebuffer(0, 0, window_size.x, window_size.y, 0, 0, window_size.x, window_size.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	ImGui::Image((ImTextureID)ID[TEXTURE], window_size, ImVec2(0,0), ImVec2(1,-1));

	//===================================================

	ImGui::PopClipRect();

	if (App->camera->frustum.type == FrustumType::PerspectiveFrustum) // TODO: extract all related with frustum to a viewport class in order to have more viewports with opengl objects
		App->camera->frustum.horizontalFov = 2.f * atanf(tanf(App->camera->frustum.verticalFov * 0.5f) * window_size.x / window_size.y);
}

void p1Scene::SelectFrameBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, ID[FBO_MS]);
	glClearColor(1.1f, 1.1f, 1.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
}

void p1Scene::DeselectFrameBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
	glClearColor(.1f, .1f, .1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}
