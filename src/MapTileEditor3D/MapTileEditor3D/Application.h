#pragma once

#include <vector>

class Module;

class Application
{
public:
	Application();
	~Application();

	bool Init();
	bool Start();
	int	 Update();
	bool CleanUp();

public:
	void AddModule(Module * const m);

private:
	std::vector<Module*> modules;
};

extern Application* App;