#pragma once

#include <string>
#include <vector>

#include <chrono>
#include <fstream>

#include "ExternalTools/JSON/json.hpp"

#define USE_PROFILER 1

#if USE_PROFILER
#define PROFILE_SECTION(name) ProfilerTimer timer##__LINE__(name)
#define PROFILE_AND_LOG(name) ProfilerTimer timer##__LINE__(name, true)
#define PROFILE_FUNCTION() PROFILE_SECTION(__FUNCTION__)
#else
#define PROFILE_SECTION(name)
#define PROFILE_FUNCTION()
#endif


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

	void InsertHeader();
	void InsertFooter();
	void Insert(const Result& result);

	static Profiler& Get() {
		static Profiler instance;
		return instance;
	}
	
private:
	std::ofstream output;
	int profileCount = 0;
};

class ProfilerTimer {
public:
	ProfilerTimer(const char* name, bool log = false);
	~ProfilerTimer();

	void Stop();

private:
	const char* name = nullptr;
	bool log = false;
	std::chrono::time_point<std::chrono::high_resolution_clock> start_timepoint;
	bool stopped = false;
};

