#pragma once

#include <string>
#include <vector>

#include "ExternalTools/JSON/json.hpp"

class Profiler
{
public:
	Profiler();
	~Profiler();

public:
	class Event {
		std::string name;
		long long start, end;
	};

	void Begin();
	void End();
	
private:
	nlohmann::json file;
};

