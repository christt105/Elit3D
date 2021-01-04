#pragma once
#include "Panels/Base/Panel.h"
#include <vector>

class p1About :
	public Panel
{
public:
	p1About(bool start_enabled = true, bool appear_mainmenubar = true, bool can_close = true);
	~p1About();

	void Update() override;

private:
	std::vector<std::string> file_data;
};

