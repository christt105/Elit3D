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
class p1DebugResources;
class p1Layers;

struct ImFont;

#define ORANGE ImVec4(1.f, 0.6470f, 0.f, 1.f)

class m1GUI :
	public Module
{
	friend class Logger;
	friend class m1Camera3D;
	friend class m1Scene; // TODO: Remove this and forward declaration
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

public: //TODO private
	std::vector<Panel*> panels;

	p1Configuration*	configuration = nullptr;
	p1About*			about = nullptr;
	p1Objects*			objects = nullptr;
	p1Inspector*		inspector = nullptr;
	p1Console*			console = nullptr;
	p1Scene*			scene = nullptr;
	p1Resources*		resources = nullptr;
	p1Tileset*			tileset = nullptr;
	p1Layers*			layers = nullptr;
	
	p1DebugResources* dbg_resources = nullptr;

	bool demo = false;
};

