#include "ExternalTools/SDL2/include/SDL.h"
#include "ExternalTools/GLEW/include/GL/glew.h"
#include "ExternalTools/SDL2/include/SDL_opengl.h"
#include <gl\glu.h>
#include <stdio.h>
#include <string>

#pragma comment(lib, "ExternalTools/SDL2/libx86/SDL2.lib")
#pragma comment(lib, "ExternalTools/SDL2/libx86/SDL2main.lib")

#pragma comment(lib, "ExternalTools/GLEW/libx86/glew32.lib")
#pragma comment(lib, "ExternalTools/GLEW/libx86/glew32s.lib")

#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "opengl32.lib")

#include <iostream>

int main(int argc, char* argv[]) {
    SDL_Window* window = nullptr;
    SDL_GLContext context;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not initialize. SDL Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    window = SDL_CreateWindow("Tile Map Editor 3D", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if (window == nullptr) {
        std::cout << "Window could not be created. SDL Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    context = SDL_GL_CreateContext(window);

    if (context == NULL) {
        std::cout << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    GLenum error = glewInit();
    if (error != GLEW_OK) {
        std::cout << "Unable to initialize OpenGL with glewInit()! Error: " << glewGetErrorString(error) << std::endl;
        return -1;
    }
    
    if (SDL_GL_SetSwapInterval(1) < 0) {
        std::cout << "Warning: Unable to set VSync! SDL Error: " << SDL_GetError() << std::endl;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    glClearColor(.23f, .57f, 1.f, 1.f);    

    std::cout << "Hello World!";

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

    return 0;
}