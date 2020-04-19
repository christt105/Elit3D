#include "m1Window.h"

#include "ExternalTools/SDL2/include/SDL_opengl.h"

#include "Logger.h"

m1Window::m1Window(bool start_enabled) : Module("Window", start_enabled)
{
}

m1Window::~m1Window()
{
}

bool m1Window::Init()
{
	bool ret = true;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        LOG("SDL could not initialize. SDL Error: %s", SDL_GetError());
        ret = false;
    }
    else {
        window = SDL_CreateWindow("Tile Map Editor 3D", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

        if (window == nullptr) {
            LOG("Window could not be created. SDL Error: %s", SDL_GetError());
            ret = false;
        }
        else {
            LOG("Window initialized");
        }
    }

	return ret;
}

bool m1Window::CleanUp()
{
    SDL_DestroyWindow(window);
    SDL_Quit();

    return true;
}
