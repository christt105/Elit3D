#pragma once

#include "Module.h"

#include <SDL_video.h>

class Shader;

class m1Render3D :
	public Module
{
	friend class m1GUI;
public:
	m1Render3D(bool start_enabled = true);
	~m1Render3D();

public:
	bool Init(const nlohmann::json& node) override;

	UpdateStatus PreUpdate() override;
	UpdateStatus PostUpdate() override;

	bool CleanUp() override;

private:
	SDL_GLContext context;

	Shader* bShader = nullptr;
};

