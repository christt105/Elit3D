#pragma once
#include "Module.h"
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
	unsigned int VAO = 0;
	unsigned int grid = 0u;
	int grid_vertex_size = 0;
};

