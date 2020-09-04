#pragma once

#include "Module.h"

#include <SDL_video.h>

class r1Shader;
class Viewport;

class m1Render3D :
	public Module
{
	friend class m1GUI;
	friend class p1Configuration;
public:
	m1Render3D(bool start_enabled = true);
	~m1Render3D();

public:
	bool Init(const nlohmann::json& node) override;

	bool Start() override;

	UpdateStatus PreUpdate() override;
	UpdateStatus PostUpdate() override;

	bool CleanUp() override;

	Viewport* CreateViewport();

	r1Shader* GetShader(const char* name);

private:
	void loadShaders();

private:
	SDL_GLContext context = nullptr;

	std::vector<Viewport*> viewports;

	std::map<std::string, unsigned int> shaders;
	std::map<std::string, r1Shader*> programs;
};

