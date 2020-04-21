#pragma once
#include "Module.h"
#include <SDL_scancode.h>

#define SDL_MAX_KEYS 300 // SDL has 512 values of SDL_Scancode but we only want to have 300 as maximum

class m1Input :
	public Module
{
	enum class KeyState {
		IDLE = 0, DOWN, REPEAT, UP
	};
public:
	m1Input(bool start_enabled = true);
	~m1Input();

public:
	UpdateStatus PreUpdate() override;

	void HandleKeyboard();

	bool IsKeyDown(SDL_Scancode scancode);
	bool IsKeyRepeating(SDL_Scancode scancode);
	bool IsKeyUp(SDL_Scancode scancode);

	bool IsKeyPressed(SDL_Scancode scancode);

private:
	KeyState* keyboard = nullptr;
};

