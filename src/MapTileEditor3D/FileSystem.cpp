#include "FileSystem.h"

#include <fstream>

#include "Logger.h"

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
    LOGE("Cannot open json with path %s", path);

	return nlohmann::json();
}

std::string FileSystem::OpenTextFile(const char* path)
{
    try {
        std::ifstream in(path, std::ios::in | std::ios::binary);
        if (in)
        {
            std::string contents;
            in.seekg(0, std::ios::end);
            contents.resize(in.tellg());
            in.seekg(0, std::ios::beg);
            in.read(&contents[0], contents.size());
            in.close();
            return contents;
        }
        throw path;
    }
    catch (...) {
        LOGE("Cannot open file with path %s", path);
        throw path;
    }
}
