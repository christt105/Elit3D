#pragma once
#include "Module.h"

class Object;
class p1Scene;

class m1Scene :
	public Module
{
public:
	m1Scene(bool start_enabled = true);
	~m1Scene();

	bool Init(const nlohmann::json& node) override;
	bool Start() override;
	void GenerateGrid();
	UpdateStatus Update() override;
	void DrawGrid();
	bool CleanUp() override;

private:
	unsigned int VAO = 0;
	unsigned int grid = 0u;
	int grid_vertex_size = 0;

	p1Scene* panel_scene = nullptr;

	Object* map[10] = { nullptr };

public:
	bool draw_grid = true;
};

