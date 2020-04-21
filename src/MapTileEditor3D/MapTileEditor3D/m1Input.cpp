#include "m1Input.h"
#include <SDL_events.h>

#include "ExternalTools/ImGui/imgui_impl_sdl.h"

#include "Logger.h"

#include "ExternalTools/mmgr/mmgr.h"

m1Input::m1Input(bool start_enabled) : Module("Input", start_enabled)
{
    keyboard = new KeyState[SDL_MAX_KEYS];
    memset(keyboard, (int)KeyState::IDLE, SDL_MAX_KEYS);
}

m1Input::~m1Input()
{
    delete[] keyboard;
}

UpdateStatus m1Input::PreUpdate()
{
    const Uint8* keys = SDL_GetKeyboardState(NULL);

    for (int i = 0; i < SDL_MAX_KEYS; ++i) {
        switch (keyboard[i])
        {
        case m1Input::KeyState::IDLE:
            if (keys[i] == 1)
                keyboard[i] = KeyState::DOWN;
            break;
        case m1Input::KeyState::DOWN:
            if (keys[i] == 1)
                keyboard[i] = KeyState::REPEAT;
            else
                keyboard[i] = KeyState::UP;
            break;
        case m1Input::KeyState::REPEAT:
            if (keys[i] == 0)
                keyboard[i] = KeyState::UP;
            break;
        case m1Input::KeyState::UP:
            if (keys[i] == 1)
                keyboard[i] = KeyState::DOWN;
            else
                keyboard[i] = KeyState::IDLE;
            break;
        default:
            break;
        }
    }

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
        switch (event.type)
        {
        case SDL_QUIT:
            return UpdateStatus::UPDATE_STOP;
            break;
        case SDL_WINDOWEVENT:
            switch (event.window.type)
            {
            case SDL_WINDOWEVENT_RESIZED:
                break;
            case SDL_WINDOWEVENT:
                break;
            default:
                break;
            }
            break;
        case SDL_KEYDOWN:
            break;
        case SDL_KEYUP:
            break;
        case SDL_MOUSEMOTION:
            break;
        case SDL_MOUSEBUTTONDOWN:
            break;
        case SDL_MOUSEBUTTONUP:
            break;
        case SDL_MOUSEWHEEL:
            break;
        case SDL_DROPFILE: {
            char* file = event.drop.file;
            LOG("Dropped %s in window", file);
            SDL_free(file);
            break;
        }
        default:
            break;
        }
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
                return UpdateStatus::UPDATE_STOP;
        }
    }

	return UpdateStatus::UPDATE_CONTINUE;
}

bool m1Input::IsKeyDown(SDL_Scancode scancode)
{
    return keyboard[scancode] == KeyState::DOWN;
}

bool m1Input::IsKeyRepeating(SDL_Scancode scancode)
{
    return keyboard[scancode] == KeyState::REPEAT;
}

bool m1Input::IsKeyUp(SDL_Scancode scancode)
{
    return keyboard[scancode] == KeyState::UP;
}

bool m1Input::IsKeyPressed(SDL_Scancode scancode)
{
    return keyboard[scancode] != KeyState::IDLE;
}
