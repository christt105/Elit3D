#pragma once
#include "Module.h"
class m1Scene :
	public Module
{
public:
	m1Scene(bool start_enabled = true);
	~m1Scene();

	bool Init() override;
	bool Start() override;
	UpdateStatus Update() override;
	bool CleanUp() override;

private:
	unsigned int VAO = 0u;

	float* vertices = nullptr;
	unsigned int VBO = 0u;
	unsigned int* indices = nullptr;
	unsigned int EBO = 0u;
};

