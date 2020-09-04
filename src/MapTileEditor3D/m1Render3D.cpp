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

#include "Profiler.h"

#include "ExternalTools/mmgr/mmgr.h"

m1Render3D::m1Render3D(bool start_enabled) : Module("Render3D", start_enabled)
{
}

m1Render3D::~m1Render3D()
{
    for (auto i = programs.begin(); i != programs.end(); ++i)
        delete (*i).second;
}

bool m1Render3D::Init(const nlohmann::json& node)
{
    PROFILE_FUNCTION();
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

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    loadShaders();

	return ret;
}

bool m1Render3D::Start()
{
    return true;
}

UpdateStatus m1Render3D::PreUpdate()
{
    PROFILE_FUNCTION();
    for (auto i = viewports.begin(); i != viewports.end(); ++i)
        (*i)->Clear();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(.1f, .1f, .1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus m1Render3D::PostUpdate()
{
    PROFILE_FUNCTION();
    SDL_GL_SwapWindow(App->window->window);

    return UpdateStatus::UPDATE_CONTINUE;
}

bool m1Render3D::CleanUp()
{
    PROFILE_FUNCTION();
    for (auto i = viewports.begin(); i != viewports.end(); ++i)
        delete* i;

    for(auto shader : shaders)
    {
        glDeleteShader(shader.second);
    }

    SDL_GL_DeleteContext(context);

    return true;
}

Viewport* m1Render3D::CreateViewport()
{
    Viewport* v = new Viewport();
    viewports.push_back(v);
    return v;
}

r1Shader* m1Render3D::GetShader(const char* name)
{
    auto p = programs.find(name);
    return (p == programs.end()) ? nullptr : (*p).second;
}

void m1Render3D::loadShaders()
{
    //load all shaders
    Folder fshaders = FileSystem::GetFilesRecursive("Configuration/Shader/Shaders/");

    for (auto shader : fshaders.files) {
        shaders[shader.first] = r1Shader::Compile(fshaders.full_path + shader.first);
    }

    //link
    std::string link = App->file_system->OpenTextFile("Configuration/Shader/shaders_link.txt");

    std::istringstream iss(link);

    for (std::string line; std::getline(iss, line); )
    {
        r1Shader* shader = new r1Shader();
        int index = 0;
        std::string name, vertex, fragment;
        for (auto i = line.begin(); i != line.end(); ++i) {
            if (*i != ' ' && *i != '\n' && *i != '\r') {
                switch (index)
                {
                case 0: // name
                    name.push_back(*i);
                    break;
                case 1: // vertex shader
                    vertex.push_back(*i);
                    break;
                case 2: // fragment shader
                    fragment.push_back(*i);
                    break;
                default:
                    break;
                }
            }
            else {
                ++index;
            }
        }
        shader->SetName(name.c_str());
        shader->Link(shaders[vertex], shaders[fragment]);
        programs[name] = shader;
    }
}
