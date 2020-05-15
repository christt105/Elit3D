#include "m1Window.h"
#include "Application.h"
#include <string>
#include <SDL_opengl.h>

#include "m1Camera3D.h"

#include "Logger.h"

#include "ExternalTools/mmgr/mmgr.h"

m1Window::m1Window(bool start_enabled) : Module("Window", start_enabled)
{
}

m1Window::~m1Window()
{
}

bool m1Window::Init(const nlohmann::json& node)
{
	bool ret = true;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        LOG("SDL could not initialize. SDL Error: %s", SDL_GetError());
        ret = false;
    }
    else {
        Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

        for (auto n = node["flags"].begin(); n != node["flags"].end(); ++n) {
            if (n.key().compare("fullscreen") == 0 && *n) {
                flags |= SDL_WINDOW_FULLSCREEN;
            }
            else if (n.key().compare("fullscreen_desktop") == 0 && *n) {
                flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
            }
            else if (n.key().compare("borderless") == 0 && *n) {
                flags |= SDL_WINDOW_BORDERLESS;
            }
            else if (n.key().compare("resizable") == 0 && *n) {
                flags |= SDL_WINDOW_RESIZABLE;
            }
            else if (n.key().compare("minimized") == 0 && *n) {
                flags |= SDL_WINDOW_MINIMIZED;
            }
            else if (n.key().compare("maximized") == 0 && *n) {
                flags |= SDL_WINDOW_MAXIMIZED;
            }
        }

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
        
        window = SDL_CreateWindow(
            App->GetName(),
            (node["x"] == -1) ? SDL_WINDOWPOS_UNDEFINED : node.value("x", 0),
            (node["y"] == -1) ? SDL_WINDOWPOS_UNDEFINED : node.value("y", 0),
            node.value("width", 640),
            node.value("height", 480),
            flags
        );

        if (window == nullptr) {
            LOG("Window could not be created. SDL Error: %s", SDL_GetError());
            ret = false;
        }
    }

	return ret;
}

bool m1Window::Start()
{
    SDL_GetWindowSize(window, &width, &height);
    SDL_GetWindowPosition(window, &x, &y);
    glViewport(0, 0, width, height);

    return true;
}

bool m1Window::CleanUp()
{
    SDL_DestroyWindow(window);
    SDL_Quit();

    return true;
}

float m1Window::GetAspectRatio() const
{
    return (float)(width / height);
}

unsigned int m1Window::GetWidth() const
{
    return width;
}

unsigned int m1Window::GetHeight() const
{
    return height;
}

void m1Window::SetWindowSize(const int& w, const int& h)
{
    width = w;
    height = h;
    glViewport(0, 0, w, h);
    App->camera->SetFov();
}
