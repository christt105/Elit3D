#include "m1Window.h"
#include "Application.h"
#include <string>
#include <SDL_opengl.h>

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
    return width / height;
}

void m1Window::SetWindowSize(const int& w, const int& h)
{
    width = w;
    height = h;
    glViewport(x, y, w, h);
}
