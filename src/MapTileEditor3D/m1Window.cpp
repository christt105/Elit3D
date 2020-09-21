#include "m1Window.h"
#include "Application.h"
#include <string>
#include <SDL_opengl.h>

#include "m1Camera3D.h"

#include "Logger.h"

#include "Profiler.h"

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
