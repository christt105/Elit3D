#pragma once
#include "Module.h"

class Object;

class m1Objects :
	public Module
{
public:
	m1Objects(bool start_enabled = true);
	~m1Objects();

	Object* CreateEmptyObject();

	bool Start() override;
	UpdateStatus Update() override;

private:
	std::vector<Object*> objects;
};

