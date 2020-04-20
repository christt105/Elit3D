#include "FileSystem.h"

#include <fstream>

FileSystem::FileSystem() 
{
}

FileSystem::~FileSystem()
{
}

nlohmann::json FileSystem::OpenJSONFile(const char* path)
{
	std::ifstream f(path);
	if (f.good()) {
		nlohmann::json j;
		f >> j;
		return j;
	}

	return nlohmann::json();
}
