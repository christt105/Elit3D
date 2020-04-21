#pragma once
#include "Module.h"
class m1GUI :
	public Module
{
public:
	m1GUI(bool start_enabled = true);
	~m1GUI();

	bool Init(const nlohmann::json& node) override;
	bool Start() override;
	UpdateStatus PreUpdate() override;
	UpdateStatus Update() override;
	UpdateStatus PostUpdate() override;
	bool CleanUp() override;
};

