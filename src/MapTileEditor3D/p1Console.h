#pragma once
#include "Panel.h"

#include "Logger.h"

#include <unordered_map>

class p1Console :
	public Panel
{
	friend class Logger;
public:
	p1Console(bool start_enabled = true, bool appear_mainmenubar = true, bool can_close = true);
	~p1Console();

	void Update() override;

private:
	std::unordered_map<std::string, LineLog*> map_logs;
	std::vector<LineLog*> logs;

	bool collapse = true;
	bool autoscroll = true;
};

