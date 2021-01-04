#pragma once
#include "Panels/Base/Panel.h"

class Viewport;

class p1Scene :
	public Panel
{
	friend class p1Configuration;
public:
	p1Scene(bool start_enabled = true, bool appear_mainmenubar = true, bool can_close = true);
	~p1Scene();

	void Start() override;
	void Update() override;

	void ShowCreateMap();

	void PopUpCreateMap();
	void PopUpLoadMap();

	void MenuBar();

private:
	Viewport* viewport = nullptr;
};
