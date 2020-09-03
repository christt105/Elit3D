#pragma once
#include "Module.h"

#include "Buffer.h"

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

	unsigned int VAOG = 0u;

	Buffer<float> vertices;
	Buffer<unsigned int> indices;
	Buffer<float> texture;

public:
	bool draw_grid = true;
};

