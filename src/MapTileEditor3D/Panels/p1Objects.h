#pragma once
#include "Panels/Base/Panel.h"

class Object;

class p1Objects :
	public Panel
{
public:
	p1Objects(bool start_active = true, bool appear_mainmenubar = true, bool can_close = true);
	~p1Objects();

	void Update() override;

private:
	void TreeNode(Object* obj);
};

