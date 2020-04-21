#include "FileSystem.h"

#include <fstream>

#include "ExternalTools/mmgr/mmgr.h"

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
		f.close();
		return j;
	}

	return nlohmann::json();
}
