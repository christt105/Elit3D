#pragma once
#include "Panel.h"

class Viewport;

class p1Scene :
	public Panel
{
public:
	p1Scene(bool start_enabled = true);
	~p1Scene();

	void Start() override;
	void Update() override;

	Viewport* viewport = nullptr;
};

