#pragma once

#include "ExternalTools/JSON/json.hpp"

struct Folder {
	Folder() {}
	Folder(const char* n);
	std::unordered_map<std::string, uint64_t> files;
	std::vector<Folder> folders;
	std::string full_path;
	std::string name;

	bool operator==(std::string path) {
		return full_path.compare(path) == 0;
	}
};

class FileSystem
{
public:
	FileSystem();
	~FileSystem();

	nlohmann::json OpenJSONFile(const char* path);
	void		   SaveJSONFile(const char* path, const nlohmann::json& file);
	std::string OpenTextFile(const char* path);

	static bool Exists(const char* path);

	static uint64_t LastTimeWrite(const char* path);

	static std::string NormalizePath(const char* path);

	bool CreateFolder(const char* path);
	bool fDeleteFile(const char* path);
	bool CopyTo(const char* source, const char* dst);

	bool IsFileInFolderRecursive(const char* file, const char* folder);

	static Folder GetFilesRecursive(const char* path);
	std::string GetFileExtension(const char* file, bool with_dot = false);
	std::string GetNameFile(const char* file, bool with_extension = false);

private:
	static void GetFiles(Folder& parent);
	bool IsFileInFolder(const char* file, const Folder& folder);
};
