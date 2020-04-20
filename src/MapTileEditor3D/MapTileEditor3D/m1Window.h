#pragma once

#include "Module.h"

#include "ExternalTools/SDL2/include/SDL.h"

class m1Window :
	public Module
{
	friend class m1GUI;
	friend class m1Render3D;
public:
	m1Window(bool start_enabled = true);
	~m1Window();

public:
	bool Init(const nlohmann::json& node) override;
	bool CleanUp() override;

private:
	SDL_Window* window = nullptr;
};

