#pragma once
#include "Panel.h"
#include "FileSystem.h"

class p1Resources :
	public Panel
{
public:
	p1Resources(bool start_enabled = true, bool appear_mainmenubar = true, bool can_close = true);
	~p1Resources();

	void Start() override;
	void Update() override;

private:
	Folder* root = nullptr;
	Folder* selected = nullptr;
};

