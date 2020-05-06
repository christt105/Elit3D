#pragma once
#include "Panel.h"
class p1About :
	public Panel
{
public:
	p1About(bool start_enabled = true);
	~p1About();

	void Update() override;
};

