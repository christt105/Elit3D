#pragma once

#include "ExternalTools/JSON/json.hpp"

struct Folder {
	friend class FileSystem;
	friend class FileWatch;
private:
	Folder() {}
	Folder(const char* n, Folder* parent = nullptr);
public:
	~Folder() {
		for (auto i = folders.begin(); i != folders.end(); ++i)
			delete* i;
	}
	std::unordered_map<std::string, uint64_t> files; // <filename, last_time_write>
	std::vector<Folder*> folders;
	std::string full_path;
	std::string name;
	Folder* parent = nullptr;

	bool operator==(std::string path) {
		return full_path.compare(path) == 0;
	}
};

class FileSystem
{
public:
	FileSystem();
	~FileSystem();

	static nlohmann::json	OpenJSONFile(const char* path);
	static void				SaveJSONFile(const char* path, const nlohmann::json& file);
	static std::string		OpenTextFile(const char* path);
	static void				SaveTextFile(const char* path, const char* file);;

	static bool Exists(const char* path);

	static uint64_t LastTimeWrite(const char* path);

	static std::string NormalizePath(const char* path);

	static bool CreateFolder(const char* path);
	static bool fDeleteFile(const char* path);
	static bool CopyTo(const char* source, const char* dst);

	static bool IsFileInFolder(const char* file, const char* folder, bool recursive = false);

	static std::string GetFileExtension(const char* file, bool with_dot = false);
	static std::string GetNameFile(const char* file, bool with_extension = false);
	static std::string GetFolder(const char* path);
	static std::string GetFullPath(const char* path);

	static Folder* GetPtrFolder(const char* folder);
	static Folder* GetRootFolder();

	static void DeleteRoot();

private:
	static Folder* GetFolders(const char* path);

private:
	static Folder* root;
};
