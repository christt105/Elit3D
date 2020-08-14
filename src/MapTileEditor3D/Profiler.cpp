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
	nlohmann::json node;
	node["name"] = "LOL";
	node["cat"] = "function";
	node["dur"] = 11111;
	node["ph"] = "X";
	node["pid"] = 0;
	node["tid"] = 0;
	node["ts"] = 0;

	file["traceEvents"].push_back(node);

	FileSystem::SaveJSONFile("profile.json", file);
}
