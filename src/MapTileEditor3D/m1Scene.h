#pragma once
#include "Module.h"

#include "int2.h"

class p1Scene;

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
};

