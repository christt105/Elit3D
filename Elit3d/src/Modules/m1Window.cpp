#include "Modules/m1Window.h"
#include "Core/Application.h"
#include <string>
#include <SDL_opengl.h>

#include "Modules/m1Camera3D.h"

#include "Tools/System/Logger.h"

#include "Tools/System/Profiler.h"

#include "ExternalTools/mmgr/mmgr.h"

m1Window::m1Window(bool start_enabled) : Module("Window", start_enabled)
{
}

m1Window::~m1Window()
{
}

bool m1Window::Init(const nlohmann::json& node)
{
    PROFILE_FUNCTION();
    bool ret = true;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        LOG("SDL could not initialize. SDL Error: %s", SDL_GetError());
        ret = false;
    }
    else {
        Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

        for (auto n = node["flags"].begin(); n != node["flags"].end(); ++n) {
            if (n.key().compare("fullscreen") == 0 && *n) {
                fullscreen = true;
                flags |= SDL_WINDOW_FULLSCREEN;
            }
            else if (n.key().compare("fullscreen_desktop") == 0 && *n) {
                fullscreen_desktop = true;
                flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
            }
            else if (n.key().compare("borderless") == 0 && *n) {
                borderless = true;
                flags |= SDL_WINDOW_BORDERLESS;
            }
            else if (n.key().compare("resizable") == 0 && *n) {
                resizable = true;
                flags |= SDL_WINDOW_RESIZABLE;
            }
            else if (n.key().compare("minimized") == 0 && *n) {
                minimized = true;
                flags |= SDL_WINDOW_MINIMIZED;
            }
            else if (n.key().compare("maximized") == 0 && *n) {
                maximized = true;
                flags |= SDL_WINDOW_MAXIMIZED;
            }

        }
        x = node.value("x", -1);
        y = node.value("y", -1);
        width = node.value("width", 640);
        height = node.value("height", 480);

        SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

        window = SDL_CreateWindow(
            App->GetName(),
            (x == -1) ? SDL_WINDOWPOS_UNDEFINED : y,
            (y == -1) ? SDL_WINDOWPOS_UNDEFINED : y,
            width,
            height,
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
    return true;
}

bool m1Window::CleanUp()
{
    PROFILE_FUNCTION();
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
}

void m1Window::UpdateWindowSize()
{
    SDL_SetWindowSize(window, width, height);
    App->camera->SetFov();
}

void m1Window::Save(nlohmann::json& node)
{
    int x, y;
    SDL_GetWindowPosition(window, &x, &y);
    node["x"] = x;
    node["y"] = y;

    SDL_GetWindowSize(window, &x, &y);
    node["width"] = x;
    node["height"] = y;

    Uint32 flags = SDL_GetWindowFlags(window);
    node["flags"]["fullscreen"]         = (bool)(flags & SDL_WINDOW_FULLSCREEN);
    node["flags"]["fullscreen_desktop"] = (bool)(flags & SDL_WINDOW_FULLSCREEN_DESKTOP);
    node["flags"]["borderless"]         = (bool)(flags & SDL_WINDOW_BORDERLESS);
    node["flags"]["resizable"]          = true;
    node["flags"]["minimized"]          = (bool)(flags & SDL_WINDOW_MINIMIZED);
    node["flags"]["maximized"]          = (bool)(flags & SDL_WINDOW_MAXIMIZED);
}
