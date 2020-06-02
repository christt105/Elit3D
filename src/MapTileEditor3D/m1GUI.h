#pragma once
#include "Module.h"

class Panel;
class p1Configuration;
class p1About;
class p1Objects;
class p1Inspector;
class p1Console;
class p1Scene;
class p1Resources;
class p1Tileset;

struct ImFont;

class m1GUI :
	public Module
{
	friend class Logger;
	friend class m1Camera3D;
public:
	m1GUI(bool start_enabled = true);
	~m1GUI();

	bool Init(const nlohmann::json& node) override;
	bool Start() override;
	UpdateStatus PreUpdate() override;
	void MainMenuBar();
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
	p1Scene* scene = nullptr;
	p1Resources* resources = nullptr;
	p1Tileset* tileset = nullptr;

	bool demo = false;
};

