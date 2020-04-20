#pragma once

#include "ExternalTools/JSON/json.hpp"

class FileSystem
{
public:
	FileSystem();
	~FileSystem();

	nlohmann::json OpenJSONFile(const char* path);
};
