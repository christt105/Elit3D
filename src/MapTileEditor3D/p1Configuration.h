#pragma once
#include "Panel.h"
class p1Configuration :
	public Panel
{
public:
	p1Configuration(bool start_enabled = true, bool appear_mainmenubar = true, bool can_close = true);
	~p1Configuration();

	void Update();
};

