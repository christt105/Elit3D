#pragma once
#include "Panel.h"

class Object;

class p1Objects :
	public Panel
{
public:
	p1Objects(bool start_active = true);
	~p1Objects();

	void Update() override;

private:
	void TreeNode(Object* obj);
};

