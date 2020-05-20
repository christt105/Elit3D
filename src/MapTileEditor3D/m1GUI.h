#pragma once
#include "Module.h"

class Panel;
class p1Configuration;
class p1About;
class p1Objects;
class p1Inspector;
class p1Console;

class m1GUI :
	public Module
{
	friend class Logger;
public:
	m1GUI(bool start_enabled = true);
	~m1GUI();

	bool Init(const nlohmann::json& node) override;
	bool Start() override;
	UpdateStatus PreUpdate() override;
	void DockSpace();
	UpdateStatus Update() override;
	UpdateStatus PostUpdate() override;
	bool CleanUp() override;

private:
	std::vector<Panel*> panels;

	p1Configuration* configuration = nullptr;
	p1About* about = nullptr;
	p1Objects* objects = nullptr;
	p1Inspector* inspector = nullptr;
	p1Console* console = nullptr;

	bool demo = false;
};

