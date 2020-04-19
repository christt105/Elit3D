#include <iostream>

#include "ExternalTools/GLEW/include/GL/glew.h"
#include <gl/GLU.h>
#include <stdio.h>
#include <string>
#include <list>

#include "ExternalTools/mmgr/mmgr.h"

#include "ExternalTools/SDL2/include/SDL.h"
#include "ExternalTools/SDL2/include/SDL_opengl.h"
#include "Logger.h"
#include "Application.h"

#pragma comment(lib, "ExternalTools/SDL2/libx86/SDL2.lib")
#pragma comment(lib, "ExternalTools/SDL2/libx86/SDL2main.lib")

#pragma comment(lib, "ExternalTools/GLEW/libx86/glew32.lib")
#pragma comment(lib, "ExternalTools/GLEW/libx86/glew32s.lib")

#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "opengl32.lib")

enum class MainState {
    EXIT_ERROR = -1,
    EXIT = 0,

    CREATION,
    START,
    UPDATE,
    FINISH
};

Application* App = nullptr;

int main(int argc, char* argv[]) {

    LOG("Starting Program...");

    MainState mainState = MainState::CREATION;
    bool running = true;
    while (running) {
        switch (mainState)
        {
        case MainState::CREATION:
            LOG("Application Creation");
            App = new Application();
            mainState = MainState::START;
            break;
        case MainState::START:
            LOG("Application Start");
            if (App->Start()) {
                mainState = MainState::UPDATE;
            }
            else {
                LOG("Application Start Fails, exiting with error");
                mainState = MainState::EXIT_ERROR;
            }
            break;
        case MainState::UPDATE:
            if (App->Update() == 1) {
                mainState = MainState::FINISH;
            }
            break;
        case MainState::FINISH:
            if (App->CleanUp()) {
                mainState = MainState::EXIT;
            }
            else {
                mainState = MainState::EXIT;
            }
            break;
        case MainState::EXIT:
            //LOG file;
            LOG("Colsing program...\nBye :)");
            running = false;
            break;
        case MainState::EXIT_ERROR:
            LOG("Exitting with errors :(");
            running = false;
            break;
        default:
            break;
        }
    }
    SDL_Window* window = nullptr;
    SDL_GLContext context;
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        LOG("SDL could not initialize. SDL Error: %s", SDL_GetError());
        return -1;
    }

    window = SDL_CreateWindow("Tile Map Editor 3D", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if (window == nullptr) {
        LOG("Window could not be created. SDL Error: %s", SDL_GetError());
        return -1;
    }
    else {
        LOG("Window initialized");
    }

    context = SDL_GL_CreateContext(window);

    if (context == NULL) {
        LOG("OpenGL context could not be created! SDL Error: %s", SDL_GetError());
        return -1;
    }

    GLenum error = glewInit();
    if (error != GLEW_OK) {
        LOG("Unable to initialize OpenGL with glewInit()! Error: %s", glewGetErrorString(error));
        return -1;
    }
    
    if (SDL_GL_SetSwapInterval(1) < 0) {
        LOG("Warning: Unable to set VSync! SDL Error: %s", SDL_GetError());
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    glClearColor(.23f, .57f, 1.f, 1.f);

    bool running = true;

    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = false;
        }

        glClear(GL_COLOR_BUFFER_BIT);
        glColor3f(1.f, 0.5f, 0.f);
        glBegin(GL_TRIANGLES);
        glVertex3f(-1.0f, -1.0f, 0.0f);
        glVertex3f( 1.0f, -1.0f, 0.0f);
        glVertex3f( 0.0f,  1.0f, 0.0f);
        glEnd();

        SDL_GL_SwapWindow(window);
    }
    
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return mainState;
}
