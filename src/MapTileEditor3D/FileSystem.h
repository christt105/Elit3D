#pragma once

#include "ExternalTools/JSON/json.hpp"

struct Folder {
	Folder() {}
	Folder(const char* n);
	std::vector<std::string> files;
	std::vector<Folder> folders;
	std::string full_path;
	std::string name;
};

class FileSystem
{
public:
	FileSystem();
	~FileSystem();

	nlohmann::json OpenJSONFile(const char* path);
	void		   SaveJSONFile(const char* path, const nlohmann::json& file);
	std::string OpenTextFile(const char* path);

	bool Exists(const char* path);

	uint64_t LastTimeWrite(const char* path);

	bool CreateFolder(const char* path);
	bool fDeleteFile(const char* path);
	bool CopyTo(const char* source, const char* dst);

	bool IsFileInFolderRecursive(const char* file, const char* folder);

	Folder GetFilesRecursive(const char* path);
	std::string GetFileExtension(const char* file, bool with_dot = false);
	std::string GetNameFile(const char* file, bool with_extension = false);

private:
	void GetFiles(Folder& parent);
	bool IsFileInFolder(const char* file, const Folder& folder);
};
