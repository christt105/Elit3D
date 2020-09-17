#include "Profiler.h"

#include "FileSystem.h"

Profiler::Profiler()
	:profileCount(0)
{
	Begin();
}

Profiler::~Profiler()
{
	End();
}

void Profiler::Begin()
{
	output.open("profile.json");
	InsertHeader();
}

void Profiler::End()
{
	InsertFooter();
	output.close();
	profileCount = 0;
}

void Profiler::InsertHeader()
{
	output << "{\"otherData\": {},\"traceEvents\":[";
	output.flush();
}

void Profiler::InsertFooter()
{
	output << "]}";
	output.flush();
}

void Profiler::Insert(const Result& result)
{
	if (profileCount++ > 0)
		output << ",";

	std::string name = result.name;
	std::replace(name.begin(), name.end(), '"', '\'');

	output << "{";
	output << "\"cat\":\"function\",";
	output << "\"dur\":" << (result.end - result.start) << ',';
	output << "\"name\":\"" << name << "\",";
	output << "\"ph\":\"X\",";
	output << "\"pid\":0,";
	output << "\"tid\":0," /*<< result.ThreadID << ","*/;
	output << "\"ts\":" << result.start;
	output << "}";

	output.flush();
}

ProfilerTimer::ProfilerTimer(const char* name, bool log)
	:name(name), stopped(false), log(log)
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

	if (log)
		LOGN("Profiling (%s) took %fms", name, (end - start) * 0.001);

	stopped = true;
}
