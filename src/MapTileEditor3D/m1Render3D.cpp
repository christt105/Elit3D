#include "m1Render3D.h"

#include <GL/glew.h>
#include <gl/GLU.h>

#include "Application.h"

#include "r1Shader.h"

#include "m1Window.h"

#include "Logger.h"

#include "FileSystem.h"

#include "Viewport.h"

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
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, node.value("major_version", 3));
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, node.value("minor_version", 3));
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);

    for (int i = 0; i < 4; ++i)
        background_color[i] = node["color"][i];

    glClearColor(background_color[0], background_color[1], background_color[2], background_color[3]);

    bShader = new r1Shader("Configuration/Shader/def_vx_shader.glsl", "Configuration/Shader/def_fg_shader.glsl");

    bShader->Use();
    bShader->SetVec3("color", float3::one);

	return ret;
}

bool m1Render3D::Start()
{
    return true;
}

UpdateStatus m1Render3D::PreUpdate()
{
    for (auto i = viewports.begin(); i != viewports.end(); ++i)
        (*i)->Clear();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(.1f, .1f, .1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
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
    for (auto i = viewports.begin(); i != viewports.end(); ++i)
        delete* i;

    SDL_GL_DeleteContext(context);

    return true;
}

Viewport* m1Render3D::CreateViewport()
{
    Viewport* v = new Viewport();
    viewports.push_back(v);
    return v;
}
