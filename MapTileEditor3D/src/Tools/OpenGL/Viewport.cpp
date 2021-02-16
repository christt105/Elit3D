#include "Tools/OpenGL/Viewport.h"

#include <GL/glew.h>
#include "Tools/OpenGL/OpenGLHelper.h"

#include "Core/Application.h"
#include "Modules/m1Camera3D.h"

Viewport::Viewport()
{
	glGenFramebuffers(1, &ID[FBO_MS]);
	glBindFramebuffer(GL_FRAMEBUFFER, ID[FBO_MS]);

	// generate texture
	glGenTextures(1, &ID[TEXTURE_MS]);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, ID[TEXTURE_MS]);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, smaa, GL_RGB, 1, 1, GL_TRUE);
	//glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	
	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, ID[TEXTURE_MS], 0);

	glGenRenderbuffers(1, &ID[RBO_MS]);
	glBindRenderbuffer(GL_RENDERBUFFER, ID[RBO_MS]);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, smaa, GL_DEPTH24_STENCIL8, 1, 1);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, ID[RBO_MS]);

	//========================================================================

	glGenFramebuffers(1, &ID[FBO]);
	glBindFramebuffer(GL_FRAMEBUFFER, ID[FBO]);

	// generate texture
	glGenTextures(1, &ID[TEXTURE]);
	glBindTexture(GL_TEXTURE_2D, ID[TEXTURE]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ID[TEXTURE], 0);

	glGenRenderbuffers(1, &ID[RBO]);
	glBindRenderbuffer(GL_RENDERBUFFER, ID[RBO]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1, 1);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, ID[RBO]);

	HANDLE_ERROR();
}

Viewport::~Viewport()
{
}

void Viewport::Clear()
{
	glBindFramebuffer(GL_FRAMEBUFFER, ID[FBO_MS]);
	glClearColor(color[0], color[1], color[2], 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Viewport::Update()
{
	// =========================================
	//=== Update tex
	//TODO: Update Camera FOV
	glBindFramebuffer(GL_FRAMEBUFFER, ID[FBO_MS]);

	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, ID[TEXTURE_MS]);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, smaa, GL_RGB, size.x, size.y, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, ID[TEXTURE_MS], 0);

	glBindRenderbuffer(GL_RENDERBUFFER, ID[RBO_MS]);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, smaa, GL_DEPTH24_STENCIL8, size.x, size.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, ID[RBO_MS]);

	//========================================================================

	glBindFramebuffer(GL_FRAMEBUFFER, ID[FBO]);

	// generate texture
	glBindTexture(GL_TEXTURE_2D, ID[TEXTURE]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ID[TEXTURE], 0);

	glBindRenderbuffer(GL_RENDERBUFFER, ID[RBO]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.x, size.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, ID[RBO]);

	glBindFramebuffer(GL_FRAMEBUFFER, ID[FBO_MS]);
}

void Viewport::UpdateSize(int x, int y)
{
	size = { x, y };
	camera->UpdateFrustum(x, y);
}

void Viewport::Blit() const
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, ID[FBO_MS]);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, ID[FBO]);
	glBlitFramebuffer(0, 0, size.x, size.y, 0, 0, size.x, size.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int Viewport::GetTexture() const
{
	return ID[TEXTURE];
}

void Viewport::Begin() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, ID[FBO_MS]);
	App->camera->UpdateShaders(camera);
	glViewport(0, 0, size.x, size.y);
}

void Viewport::End() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
