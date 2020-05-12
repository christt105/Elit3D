#pragma once

#include "Module.h"

#include <SDL_video.h>

class r1Shader;

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

	r1Shader* bShader = nullptr;
private:
	SDL_GLContext context = nullptr;

};

