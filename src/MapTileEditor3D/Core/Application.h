#pragma once

#include <vector>
#include <string>

#include "Tools/DebugVars.h"
#include "Tools/System/SystemInfo.h"
#include "Tools/Timer.h"

enum class UpdateStatus;

class Module;
class m1Window;
class m1Render3D;
class m1Input;
class m1Scene;
class m1GUI;
class m1Objects;
class m1Camera3D;
class m1Resources;
class m1Importer;
class m1Events;
class m1MapEditor;

class Application
{
public:
	Application();
	~Application();

	bool Init();
	bool Start();
	void PrepareUpdate();
	UpdateStatus Update();
	void FinishUpdate();
	bool CleanUp();

public:
	m1Window*		window = nullptr;
	m1Render3D*		render = nullptr;
	m1Input*		input  = nullptr;
	m1Scene*		scene = nullptr;
	m1GUI*			gui = nullptr;
	m1Objects*		objects = nullptr;
	m1Camera3D*		camera = nullptr;
	m1Resources*	resources = nullptr;
	m1Importer*		importer = nullptr;
	m1Events*		events = nullptr;
	m1MapEditor*	map_editor = nullptr;

	DebugVars debug;
	SystemInfo sys_info;

public:
	const char* GetName();
	const char* GetName() const;

	const char* GetVersion();
	const char* GetVersion() const;

	float GetDt() const;
	unsigned int GetFrames() const;
	inline unsigned int GetFrameRateLS() const {
		return framerate_last_second;
	}

	void SaveConfiguration();
	void LoadConfiduration(const char* file);

private:
	std::vector<Module*> modules;

	std::string name;
	std::string version;

	float dt = 0.f;
	uint64_t time = 0ULL;
	unsigned int framerate = 0u; //TODO: save this in a vector
	unsigned int framerate_last_second = 0u;
	uint64_t last_time = 0ULL;
	unsigned int frame_count = 0U;
	Timer framerate_last_second_timer;
};

extern Application* App;