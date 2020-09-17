#pragma once
#include "Panel.h"
class p1Inspector :
	public Panel
{
public:
	p1Inspector(bool start_enabled = true, bool appear_mainmenubar = true, bool can_close = true);
	~p1Inspector();

	void Update() override;
};

