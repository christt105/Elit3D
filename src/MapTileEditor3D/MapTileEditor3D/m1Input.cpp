#include "m1Input.h"
#include <SDL_events.h>

#include "ExternalTools/ImGui/imgui_impl_sdl.h"

m1Input::m1Input(bool start_enabled) : Module("Input", start_enabled)
{
}

m1Input::~m1Input()
{
}

UpdateStatus m1Input::PreUpdate()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT)
            return UpdateStatus::UPDATE_STOP;
    }

	return UpdateStatus::UPDATE_CONTINUE;
}
