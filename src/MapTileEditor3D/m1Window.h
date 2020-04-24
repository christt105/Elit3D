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
	bool Start() override;
	bool CleanUp() override;

	void SetWindowSize(const int& w, const int& h);

private:
	SDL_Window* window = nullptr;

	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;
};

