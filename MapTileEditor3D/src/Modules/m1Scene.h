#pragma once
#include "Modules/Base/Module.h"

#include "Tools/Math/int2.h"

class p1Scene;
class Viewport;

class m1Scene :
	public Module
{
public:
	m1Scene(bool start_enabled = true);
	~m1Scene();

	bool Init(const nlohmann::json& node) override;
	bool Start() override;
	UpdateStatus Update() override;
	bool CleanUp() override;

private:
	p1Scene* panel_scene = nullptr;

public:
	bool draw_grid = true;
	Viewport* sceneViewport = nullptr;
};

