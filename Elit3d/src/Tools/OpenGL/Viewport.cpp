#include "Tools/OpenGL/Viewport.h"

#include <GL/glew.h>
#include "Tools/OpenGL/OpenGLHelper.h"

#include "Core/Application.h"
#include "Modules/m1Render3D.h"
#include "Modules/m1Camera3D.h"
#include "Resources/r1Shader.h"

#include "Tools/System/Logger.h"

#include "ExternalTools/ImGui/imgui.h"

Viewport::Viewport()
{
	glGenTextures(1, &ID[NORMAL_TEXTURE]);
	glGenTextures(1, &ID[DEPTH_TEXTURE]);

	glGenFramebuffers(1, &ID[NORMAL_FBO]);
	glGenFramebuffers(1, &ID[MULTISAMPLING_FBO]);

	glGenRenderbuffers(1, &ID[NORMAL_DEPTH_RBO]);
	glGenRenderbuffers(1, &ID[MULTISAMPLING_COLOR_RBO]);
	glGenRenderbuffers(1, &ID[MULTISAMPLING_DEPTH_RBO]);

	HANDLE_ERROR();
}

Viewport::~Viewport()
{
	glDeleteTextures(1, &ID[NORMAL_TEXTURE]);
	glDeleteTextures(1, &ID[DEPTH_TEXTURE]);

	glDeleteFramebuffers(1, &ID[NORMAL_FBO]);
	glDeleteFramebuffers(1, &ID[MULTISAMPLING_FBO]);

	glDeleteRenderbuffers(1, &ID[NORMAL_DEPTH_RBO]);
	glDeleteRenderbuffers(1, &ID[MULTISAMPLING_COLOR_RBO]);
	glDeleteRenderbuffers(1, &ID[MULTISAMPLING_DEPTH_RBO]);
}

void Viewport::Clear()
{
	HANDLE_ERROR();
	glBindFramebuffer(GL_FRAMEBUFFER, ID[MULTISAMPLING_FBO]);
	HANDLE_ERROR();
	glClearColor(color[0], color[1], color[2], 1.0f);
	HANDLE_ERROR();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	HANDLE_ERROR();
}

void Viewport::Update()
{
	glBindTexture(GL_TEXTURE_2D, ID[NORMAL_TEXTURE]);

	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glBindTexture(GL_TEXTURE_2D, 0);

	// Depth & Stencil -------------------------------------

	glBindRenderbuffer(GL_RENDERBUFFER, ID[NORMAL_DEPTH_RBO]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.x, size.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// Frame -----------------------------------------------

	glBindFramebuffer(GL_FRAMEBUFFER, ID[NORMAL_FBO]);

	// Attachment ------------------------------------------

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ID[NORMAL_TEXTURE], 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, ID[NORMAL_DEPTH_RBO]);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		LOGW("FRAMEBUFFER NOT COMPLETED");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Multisample ================================================================

	// Color -----------------------------------------------

	glBindRenderbuffer(GL_RENDERBUFFER, ID[MULTISAMPLING_COLOR_RBO]);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, App->render->smaa, GL_RGBA8, size.x, size.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// Depth & Stencil -------------------------------------

	glBindRenderbuffer(GL_RENDERBUFFER, ID[MULTISAMPLING_DEPTH_RBO]);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, App->render->smaa, GL_DEPTH24_STENCIL8, size.x, size.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);


	// Frame -----------------------------------------------

	glBindFramebuffer(GL_FRAMEBUFFER, ID[MULTISAMPLING_FBO]);

	// Attachment ------------------------------------------

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, ID[MULTISAMPLING_COLOR_RBO]);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, ID[MULTISAMPLING_DEPTH_RBO]);


	GLenum status_2 = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status_2 != GL_FRAMEBUFFER_COMPLETE)
		LOGW("FRAMEBUFFER NOT COMPLETED");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	HANDLE_ERROR();
}

void Viewport::UpdateSize(int x, int y)
{
	size = { x, y };
	camera->UpdateFrustum(x, y);
}

void Viewport::Blit() const
{
	// Blit Frame buffer -------------------------------------------------------

	glBindFramebuffer(GL_READ_FRAMEBUFFER, ID[MULTISAMPLING_FBO]);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, ID[NORMAL_FBO]);

	glBlitFramebuffer(0, 0, size.x, size.y, 0, 0, size.x, size.y, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	// Generate Mipmap --------------------------------------------------------

	glBindTexture(GL_TEXTURE_2D, ID[NORMAL_TEXTURE]);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	HANDLE_ERROR();
}

unsigned int Viewport::GetTexture() const
{
	return ID[NORMAL_TEXTURE];
}

void Viewport::Begin() const
{
	HANDLE_ERROR();
	glBindFramebuffer(GL_FRAMEBUFFER, ID[MULTISAMPLING_FBO]);
	App->camera->UpdateShaders(camera);
	glViewport(0, 0, size.x, size.y);
	HANDLE_ERROR();
}

void Viewport::End() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Viewport::RenderOnImGui()
{
	camera->is_active = ImGui::IsWindowHovered();

	ImGui::PushClipRect(ImGui::GetWindowPos(), ImGui::GetWindowPos() + ImGui::GetWindowSize(), false);
	ImGui::SetCursorScreenPos(ImGui::GetWindowPos());

	ImVec2 window_size = ImGui::GetContentRegionAvail();
	UpdateSize((int)window_size.x, (int)window_size.y);

	Blit();
	Update();
	HANDLE_ERROR();

	ImGui::Image((ImTextureID)GetTexture(), window_size, ImVec2(0, 0), ImVec2(1, -1));

	ImGui::PopClipRect();
}

void Viewport::DrawGrid()
{
	Begin();

	static auto shader1 = App->render->GetShader("grid");
	shader1->Use();
	oglh::DepthEnable(true);
	oglh::DrawArrays(6);
	oglh::DepthEnable(false);

	End();
}
