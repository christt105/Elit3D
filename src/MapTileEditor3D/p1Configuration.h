#pragma once
#include "Panel.h"
class p1Configuration :
	public Panel
{
public:
	p1Configuration(bool start_enabled = true);
	~p1Configuration();

	void Update();
};

