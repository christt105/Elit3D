#include "m1Input.h"
#include <SDL_events.h>

#include "ExternalTools/ImGui/imgui_impl_sdl.h"

#include "Application.h"
#include "m1Window.h"
#include "m1Importer.h"

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
    SDL_PumpEvents();

    HandleKeyboard();

    for (int i = 0; i < 3; ++i) {
        if (mouse[i] == KeyState::DOWN) {
            mouse[i] = KeyState::REPEAT;
        }
        else if (mouse[i] == KeyState::UP) {
            mouse[i] = KeyState::IDLE;
        }
    }

    SDL_GetMouseState(&mouseX, &mouseY);
    mouseZ = 0;

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
        switch (event.type)
        {
        case SDL_QUIT:
            return UpdateStatus::UPDATE_STOP;
        case SDL_WINDOWEVENT:
            switch (event.window.event)
            {
            case SDL_WINDOWEVENT_RESIZED:
                App->window->SetWindowSize(event.window.data1, event.window.data2);
                break;
            case SDL_WINDOWEVENT_SIZE_CHANGED:
                App->window->SetWindowSize(event.window.data1, event.window.data2);
                break;
            default:
                break;
            }
            break;
        case SDL_MOUSEMOTION:
            
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (mouse[event.button.button-1] == KeyState::IDLE || mouse[event.button.button-1] == KeyState::UP) {
                mouse[event.button.button-1] = KeyState::DOWN;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            mouse[event.button.button-1] = KeyState::UP;
            break;
        case SDL_MOUSEWHEEL:
            mouseZ = event.wheel.y;
            break;
        case SDL_DROPFILE: {
            char* file = event.drop.file;
            LOG("Importing dropped file %s", file);
            App->importer->Import(file);
            SDL_free(file);
            break;
        }
        default:
            break;
        }
    }

	return UpdateStatus::UPDATE_CONTINUE;
}

void m1Input::HandleKeyboard()
{
    const Uint8* keys = SDL_GetKeyboardState(NULL);

    for (int i = 0; i < SDL_MAX_KEYS; ++i) {
        if (keys[i] == 1) {
            if (keyboard[i] == KeyState::IDLE)
                keyboard[i] = KeyState::DOWN;
            else if (keyboard[i] == KeyState::DOWN)
                keyboard[i] = KeyState::REPEAT;
            else if (keyboard[i] == KeyState::UP)
                keyboard[i] = KeyState::DOWN;
        }
        else {
            if (keyboard[i] == KeyState::REPEAT || keyboard[i] == KeyState::DOWN)
                keyboard[i] = KeyState::UP;
            else
                keyboard[i] = KeyState::IDLE;
        }
    }
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

bool m1Input::IsMouseButtonDown(const int& button)
{
    return mouse[button-1] == KeyState::DOWN;
}

bool m1Input::IsMouseButtonRepeating(const int& button)
{
    return mouse[button-1] == KeyState::REPEAT;
}

bool m1Input::IsMouseButtonUp(const int& button)
{
    return mouse[button-1] == KeyState::UP;
}

bool m1Input::IsMouseButtonPressed(const int& button)
{
    return mouse[button-1] != KeyState::IDLE;
}

int m1Input::GetMouseX()
{
    return mouseX;
}

int m1Input::GetMouseY()
{
    return mouseY;
}

int m1Input::GetMouseZ()
{
	return mouseZ;
}

void m1Input::GetMousePosition(int* x, int* y)
{
    *x = mouseX;
    *y = mouseY;
}
