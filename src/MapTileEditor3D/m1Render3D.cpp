#include "m1Render3D.h"

#include <GL/glew.h>
#include <gl/GLU.h>

#include "Application.h"

#include "Shader.h"

#include "m1Window.h"

#include "Logger.h"

#include "FileSystem.h"

#include "ExternalTools/MathGeoLib/include/Math/float4x4.h"
#include "ExternalTools/MathGeoLib/include/Math/Quat.h"

#include "ExternalTools/mmgr/mmgr.h"

m1Render3D::m1Render3D(bool start_enabled) : Module("Render3D", start_enabled)
{
}

m1Render3D::~m1Render3D()
{
    delete bShader;
}

bool m1Render3D::Init(const nlohmann::json& node)
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
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, node.value("major_version", 3));
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, node.value("minor_version", 3));
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    glClearColor(node["color"][0], node["color"][1], node["color"][2], node["color"][3]);

    bShader = new Shader("Shaders/def_vx_shader.glsl", "Shaders/def_fg_shader.glsl");

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);

    bShader->Use();

	return ret;
}

UpdateStatus m1Render3D::PreUpdate()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    bShader->Use();

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
