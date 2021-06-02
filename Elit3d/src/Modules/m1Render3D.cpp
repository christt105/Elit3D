#include "Modules/m1Render3D.h"

#include <GL/glew.h>
#include <gl/GLU.h>

#include "Core/Application.h"

#include "Resources/r1Shader.h"

#include "Modules/m1Window.h"
#include "Modules/m1Camera3D.h"

#include "Tools/System/Logger.h"
#include "Tools/OpenGL/OpenGLHelper.h"
#include "Tools/FileSystem.h"

#include "Tools/OpenGL/Viewport.h"

#include "Tools/System/Profiler.h"

#include "ExternalTools/mmgr/mmgr.h"

#ifdef _WIN32
#include <windows.h>
#else
#define APIENTRY
#endif
m1Render3D::m1Render3D(bool start_enabled) : Module("Render3D", start_enabled)
{
}

m1Render3D::~m1Render3D()
{
    for (auto i = programs.begin(); i != programs.end(); ++i)
        delete (*i).second;
}

void APIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* msg, const void* data);

bool m1Render3D::Init(const nlohmann::json& node)
{
    PROFILE_FUNCTION();
	bool ret = true;

    use_multisampling = node.value("use_multisampling", false);
    smaa = node.value("smaa", 4);
    
    //TODO: Fix texture issue on OpenGL version changes
    int major = node.value("major_version", 3);
    int minor = node.value("minor_version", 1);
    if (major > 0 && minor >= 0) {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor);
    }

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
    glCullFace(GL_FRONT);

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDepthFunc(GL_LEQUAL);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, node.value("major_version", 3));
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, node.value("minor_version", 3));

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);

    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(GLDebugMessageCallback, nullptr);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    LoadShaders();

    HANDLE_ERROR();

	return ret;
}

bool m1Render3D::Start()
{
    return true;
}

UpdateStatus m1Render3D::PreUpdate()
{
    PROFILE_FUNCTION();

    for (auto i = viewports.begin(); i != viewports.end(); ++i) {
        HANDLE_ERROR();
        (*i).second->Clear();
        HANDLE_ERROR();
        if ((*i).second->drawGrid)
            (*i).second->DrawGrid();
    }

    return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus m1Render3D::PostUpdate()
{
    PROFILE_FUNCTION();
    SDL_GL_SwapWindow(App->window->window);
    HANDLE_ERROR();

    return UpdateStatus::UPDATE_CONTINUE;
}

bool m1Render3D::CleanUp()
{
    PROFILE_FUNCTION();
    for (auto i = viewports.begin(); i != viewports.end(); ++i)
        delete (*i).second;

    for(auto shader : shaders)
    {
        glDeleteShader(shader.second);
    }

    HANDLE_ERROR();

    SDL_GL_DeleteContext(context);

    return true;
}

void m1Render3D::Save(nlohmann::json& node)
{
    node["minor_version"] = 3;
    node["major_version"] = 3;
}

Viewport* m1Render3D::CreateViewport(const char* name)
{
    Viewport* v = new Viewport();
    v->camera = App->camera->CreateCamera(name);
    viewports[name] = v;
    return v;
}

Viewport* m1Render3D::GetViewport(const char* name)
{
    return viewports[name];
}

r1Shader* m1Render3D::GetShader(const char* name)
{
    auto p = programs.find(name);
    return (p == programs.end()) ? nullptr : (*p).second;
}

void m1Render3D::LoadShaders()
{
    //load all shaders
    Folder* fshaders = FileSystem::GetPtrFolder((FileSystem::sAppdata + "Configuration/Shader/Shaders/").c_str(), true);

    for (auto shader : fshaders->files) {
        shaders[shader.first] = r1Shader::Compile(fshaders->full_path + shader.first);
    }

    //link
    std::string link = FileSystem::OpenTextFile((FileSystem::sAppdata + "Configuration/Shader/shaders_link.txt").c_str());

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

void APIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei length,
    const GLchar* msg, const void* data)
{
    const char* _source;
    const char* _type;
    const char* _severity;

    switch (source) {
    case GL_DEBUG_SOURCE_API:
        _source = "API";
        break;

    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        _source = "WINDOW SYSTEM";
        break;

    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        _source = "SHADER COMPILER";
        break;

    case GL_DEBUG_SOURCE_THIRD_PARTY:
        _source = "THIRD PARTY";
        break;

    case GL_DEBUG_SOURCE_APPLICATION:
        _source = "APPLICATION";
        break;

    case GL_DEBUG_SOURCE_OTHER:
        _source = "UNKNOWN";
        break;

    default:
        _source = "UNKNOWN";
        break;
    }

    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        _type = "ERROR";
        break;

    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        _type = "DEPRECATED BEHAVIOR";
        break;

    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        _type = "UDEFINED BEHAVIOR";
        break;

    case GL_DEBUG_TYPE_PORTABILITY:
        _type = "PORTABILITY";
        break;

    case GL_DEBUG_TYPE_PERFORMANCE:
        _type = "PERFORMANCE";
        break;

    case GL_DEBUG_TYPE_OTHER:
        _type = "OTHER";
        break;

    case GL_DEBUG_TYPE_MARKER:
        _type = "MARKER";
        break;

    default:
        _type = "UNKNOWN";
        break;
    }

    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        _severity = "HIGH";
        break;

    case GL_DEBUG_SEVERITY_MEDIUM:
        _severity = "MEDIUM";
        break;

    case GL_DEBUG_SEVERITY_LOW:
        _severity = "LOW";
        break;

    case GL_DEBUG_SEVERITY_NOTIFICATION:
        _severity = "NOTIFICATION";
        break;

    default:
        _severity = "UNKNOWN";
        break;
    }

    // ignore notification severity (you can add your own ignores)
    // + Adds __debugbreak if _DEBUG is defined (automatic in visual studio)
    // note: __debugbreak is specific for MSVC, won't work with gcc/clang
    // -> in that case remove it and manually set breakpoints
    if (_severity != "NOTIFICATION") {
        LOGE("OpenGL error [%d]: %s of %s severity, raised from %s: %s",
            id, _type, _severity, _source, msg);
#ifdef _DEBUG
        
#endif
    }
}