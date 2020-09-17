#pragma once
#include "Panel.h"
#include "FileSystem.h"

class p1Resources :
	public Panel
{
public:
	p1Resources(bool start_enabled = true, bool appear_mainmenubar = true, bool can_close = true);
	~p1Resources();

	void Update() override;

private:
	void DisplayFolder(const Folder& folder, bool default_open);

	Folder root;
};

