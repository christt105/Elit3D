#pragma once

#include "Module.h"

#include <SDL_video.h>

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

	const char* vertexShaderSource = nullptr;
	unsigned int fragmentShader = 0u;
	const char* fragmentShaderSource = nullptr;
	unsigned int vertexShader = 0u;

	int shaderProgram = 0;
};

