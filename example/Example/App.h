#pragma once

struct SDL_Window;

class App
{
public:
	App();
	~App();

	void Init();
	void CleanUp();

	void Update();

	bool quit = false;

private:
	SDL_Window* window = nullptr;
	void* context = nullptr;
};

