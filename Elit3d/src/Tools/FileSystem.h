#pragma once

#include "ExternalTools/JSON/json.hpp"

struct Folder {
	friend class FileSystem;
	friend class FileWatch;
private:
	Folder() = default;
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

	bool operator==(const std::string& path) const {
		return full_path.compare(path) == 0;
	}
};

class FileSystem
{
public:
	static nlohmann::json	OpenJSONFile(const char* path);
	static void				SaveJSONFile(const char* path, const nlohmann::json& file);
	static std::string		OpenTextFile(const char* path);
	static void				SaveTextFile(const char* path, const char* file);;

	static bool Exists(const char* path);

	static uint64_t LastTimeWrite(const char* path);

	static std::string NormalizePath(const std::string& path);

	static bool CreateFolder(const char* path);
	static bool fDeleteFile(const char* path);
	static bool DeleteFolder(const char* path);
	static bool CopyTo(const char* source, const char* dst);
	static bool MoveTo(const char* source, const char* dst);

	//TODO static bool IsFileInFolder(const char* file, const char* folder, bool recursive = false);

	static std::string GetFileExtension(const char* file, bool with_dot = false);
	static std::string GetNameFile(const char* file, bool with_extension = false);
	static std::string GetFolder(const char* path);
	static std::string GetNameFolder(const char* path, bool with_slash = true);
	static std::string GetParentFolder(const char* path);
	static std::string GetFullPath(const char* path);

	static Folder* GetPtrFolder(const char* folder, bool is_appdata = false);
	static Folder* GetRootFolder();
	static Folder* RegenerateRootFolder();
	static void GenerateFolders();

	static void DeleteRoot();

private:
	static Folder* GetFolders(const char* path);

public:
	static std::string sAppdata;

private:
	static Folder* root;
	static Folder* appdata;
};
