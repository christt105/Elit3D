#pragma once
#include "Panel.h"

class Viewport;

class p1Scene :
	public Panel
{
public:
	p1Scene(bool start_enabled = true, bool appear_mainmenubar = true, bool can_close = true);
	~p1Scene();

	void Start() override;
	void Update() override;

	void ShowCreateMap();

	void PopUpCreateMap();

	void MenuBar();

	Viewport* viewport = nullptr;
};
