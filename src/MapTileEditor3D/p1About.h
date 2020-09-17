#pragma once
#include "Panel.h"
class p1About :
	public Panel
{
public:
	p1About(bool start_enabled = true, bool appear_mainmenubar = true, bool can_close = true);
	~p1About();

	void Update() override;
};

