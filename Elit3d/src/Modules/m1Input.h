#pragma once
#include "Modules/Base/Module.h"
#include <SDL_scancode.h>
#include "ExternalTools/SDL2/include/SDL_mouse.h"

#define SDL_MAX_KEYS 231 // SDL has 512 values of SDL_Scancode but we only want to have 231 as maximum

class int2;

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

	bool IsKeyDown(SDL_Scancode scancode) const;
	bool IsKeyRepeating(SDL_Scancode scancode) const;
	bool IsKeyUp(SDL_Scancode scancode) const;

	bool IsKeyPressed(SDL_Scancode scancode) const;

	bool IsMouseButtonDown(const int& button) const;
	bool IsMouseButtonRepeating(const int& button) const;
	bool IsMouseButtonUp(const int& button) const;

	bool IsMouseButtonPressed(const int& button) const;

	int GetMouseX() const;
	int GetMouseY() const;
	int GetMouseZ() const;

	void GetMousePosition(int* x, int* y) const;
	int2 GetMousePosition() const;

private:
	KeyState* keyboard = nullptr;
	KeyState  mouse[3] = {KeyState::IDLE};
	int mouseX = 0;
	int mouseY = 0;
	int mouseZ = 0;
};
