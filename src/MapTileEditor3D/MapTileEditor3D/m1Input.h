#pragma once
#include "Module.h"
class m1Input :
	public Module
{
public:
	m1Input(bool start_enabled = true);
	~m1Input();
public:
	UpdateStatus PreUpdate() override;
};

