#pragma once

#include <vector>
#include <string>

#include "DebugVars.h"

#include <SDL_stdinc.h>

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

class FileSystem;
class Random;

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

	FileSystem* file_system = nullptr;
	Random*		random = nullptr;

	DebugVars debug;

public:
	const char* GetName();
	const char* GetName() const;

	const char* GetVersion();
	const char* GetVersion() const;

	float GetDt() const;
	unsigned int GetFrames() const;
	inline unsigned char GetFrameRate() const {
		return framerate;
	}

	void ExecuteURL(const char* url);

private:
	std::vector<Module*> modules;

	std::string name;
	std::string version;

	float dt = 0.f;
	Uint64 time = 0ULL;
	unsigned char framerate = 0u; //TODO: save this in a vector
	Uint64 last_time = 0ULL;
	unsigned int frame_count = 0U;
};

extern Application* App;