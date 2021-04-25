#include "App.h"

#include <iostream>
#include <SDL.h>
#include <GL/glew.h>

App::App()
{
}

App::~App()
{
}

void App::Init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not initialize. SDL Error: " << SDL_GetError();
        quit = true;
        return;
    }
    Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_MAXIMIZED;

    SDL_Window* window = SDL_CreateWindow(
        "Elit3D Example",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        -1,
        -1,
        flags
    );

    if (window == nullptr) {
        std::cout << "Window could not be created. SDL Error: " << SDL_GetError();
        quit = true;
        return;
    }

    SDL_GLContext glContext = SDL_GL_CreateContext(window);

    if (glContext == nullptr) {
        std::cout << "OpenGL context could not be created! SDL Error: " << SDL_GetError();
        quit = true;
        return;
    }

    GLenum error = glewInit();
    if (error != GLEW_OK) {
        std::cout << "Unable to initialize OpenGL with glewInit()! Error: " << glewGetErrorString(error);
        quit = true;
        return;
    }

    if (SDL_GL_SetSwapInterval(1) < 0) {
        std::cout << "Warning: Unable to set VSync! SDL Error: %s" << SDL_GetError();
    }
}

void App::CleanUp()
{
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void App::Update()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            quit = true;
            break;
        default:
            break;
        }
    }

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    SDL_GL_SwapWindow(window);
}
