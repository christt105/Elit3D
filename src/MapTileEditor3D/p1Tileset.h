#pragma once
#include "Panel.h"
class p1Tileset :
	public Panel
{
public:
	p1Tileset(bool start_enabled = true);
	~p1Tileset();

	void Update() override;
};

