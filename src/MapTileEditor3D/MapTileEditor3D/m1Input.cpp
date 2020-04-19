#include "m1Input.h"
#include "ExternalTools/SDL2/include/SDL_events.h"

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
        if (event.type == SDL_QUIT)
            return UpdateStatus::UPDATE_STOP;
    }

	return UpdateStatus::UPDATE_CONTINUE;
}
