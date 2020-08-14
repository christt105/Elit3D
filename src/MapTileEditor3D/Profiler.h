#pragma once

#include <string>
#include <vector>

#include <chrono>

#include "ExternalTools/JSON/json.hpp"

#define PROFILE_SECTION(name) ProfilerTimer timer##__LINE__(name)
#define PROFILE_FUNCTION() PROFILE_SECTION(__FUNCTION__)


class Profiler
{
public:
	Profiler();
	~Profiler();

public:
	struct Result {
		std::string name;
		long long start, end;
	};

	void Begin();
	void End();
	void Insert(const Result& result);

	static Profiler& Get();
	
private:
	nlohmann::json file;
};

class ProfilerTimer {
public:
	ProfilerTimer(const char* name);
	~ProfilerTimer();

	void Stop();

private:
	const char* name = nullptr;
	std::chrono::time_point<std::chrono::high_resolution_clock> start_timepoint;
	bool stopped = false;
};

