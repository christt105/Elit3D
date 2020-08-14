#include "Profiler.h"

#include "FileSystem.h"

Profiler::Profiler()
{
	Begin();
}

Profiler::~Profiler()
{
	End();
}

void Profiler::Begin()
{
	file.clear();
	file["otherData"] = nlohmann::json::object();
	file["traceEvents"] = nlohmann::json::array();
}

void Profiler::End()
{
	FileSystem::SaveJSONFile("profile.json", file);
}

void Profiler::Insert(const Result& result)
{
	nlohmann::json node;

	node["name"] = result.name;
	node["dur"]	 = (result.end - result.start);
	node["ts"]   = result.start;
	node["cat"]  = "function";
	node["ph"]   = "X";
	node["pid"]  = 0;
	node["tid"]  = 0;

	file["traceEvents"].push_back(node);
}

Profiler& Profiler::Get()
{
	static Profiler instance;
	return instance;
}

ProfilerTimer::ProfilerTimer(const char* name)
	:name(name), stopped(false)
{
	start_timepoint = std::chrono::high_resolution_clock::now();
}

ProfilerTimer::~ProfilerTimer()
{
	if (!stopped)
		Stop();
}

void ProfilerTimer::Stop()
{
	auto end_timepoint = std::chrono::high_resolution_clock::now();

	long long start = std::chrono::time_point_cast<std::chrono::microseconds>(start_timepoint).time_since_epoch().count();
	long long end = std::chrono::time_point_cast<std::chrono::microseconds>(end_timepoint).time_since_epoch().count();

	//uint32_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
	Profiler::Get().Insert({ name, start, end });

	stopped = true;
}
