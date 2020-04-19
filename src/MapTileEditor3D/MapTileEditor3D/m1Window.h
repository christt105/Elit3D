#pragma once

#include "Module.h"

#include "ExternalTools/SDL2/include/SDL.h"

class m1Window :
	public Module
{
	friend class m1Render3D;
public:
	m1Window(bool start_enabled = true);
	~m1Window();

public:
	bool Init() override;
	bool CleanUp() override;

private:
	SDL_Window* window = nullptr;
};

