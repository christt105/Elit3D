#include "m1Render3D.h"

#include "ExternalTools/GLEW/include/GL/glew.h"
#include <gl/GLU.h>

#include "Application.h"

#include "m1Window.h"

#include "Logger.h"

#pragma comment(lib, "ExternalTools/GLEW/libx86/glew32.lib")
#pragma comment(lib, "ExternalTools/GLEW/libx86/glew32s.lib")

#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "opengl32.lib")

m1Render3D::m1Render3D(bool start_enabled) : Module("Render3D", start_enabled)
{
}

m1Render3D::~m1Render3D()
{
}

bool m1Render3D::Init()
{
	bool ret = true;

    context = SDL_GL_CreateContext(App->window->window);

    if (context == NULL) {
        LOG("OpenGL context could not be created! SDL Error: %s", SDL_GetError());
        ret = false;
    }

    GLenum error = glewInit();
    if (error != GLEW_OK) {
        LOG("Unable to initialize OpenGL with glewInit()! Error: %s", glewGetErrorString(error));
        ret = false;
    }

    if (SDL_GL_SetSwapInterval(1) < 0) {
        LOG("Warning: Unable to set VSync! SDL Error: %s", SDL_GetError());
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    glClearColor(.23f, .57f, 1.f, 1.f);

	return ret;
}

UpdateStatus m1Render3D::PreUpdate()
{
    glClear(GL_COLOR_BUFFER_BIT);
    return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus m1Render3D::PostUpdate()
{
    SDL_GL_SwapWindow(App->window->window);

    return UpdateStatus::UPDATE_CONTINUE;
}

bool m1Render3D::CleanUp()
{
    SDL_GL_DeleteContext(context);
    return true;
}
