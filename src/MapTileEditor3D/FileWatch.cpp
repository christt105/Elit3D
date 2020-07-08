#include "FileWatch.h"
#include "Logger.h"
#include "Application.h"
#include "Globals.h"

#include <filesystem>

#include "ExternalTools/mmgr/mmgr.h"

namespace fs = std::filesystem;

FileWatch::FileWatch()
{
}

FileWatch::~FileWatch()
{
	watch = false;
	if (fut.valid())
		fut.wait();
}

void FileWatch::Subscribe(const char* folder, bool recursive)
{
	this->folder = folder;
}

void FileWatch::StartWatching()
{
#if USE_FILEWATCHER
	root = App->file_system->GetFilesRecursive(folder);
	
	fut = std::async(std::launch::async, &FileWatch::Watch, this);
#endif
}

void FileWatch::Watch()
{
	while (watch) {
		CheckFolder(root.full_path.c_str(), root);
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

void FileWatch::CheckFolder(const char* folder, Folder& f)
{
	auto i = f.files.begin();
	while (i != f.files.end()) {
		if (!FileSystem::Exists((f.full_path + (*i).first).c_str())) {
			LOG("File %s deleted", (*i).first.c_str());
			i = f.files.erase(i);
		}
		else
			++i;
	}

	for (const auto& entry : fs::directory_iterator(folder)) {
		if (entry.is_directory()) {
			auto it = std::find(f.folders.begin(), f.folders.end(), entry.path().u8string() + "/");
			if (it != f.folders.end())
				CheckFolder(entry.path().u8string().c_str(), *it);
		}
		else {
			struct stat time;
			stat(entry.path().u8string().c_str(), &time);
			auto file = f.files.find(entry.path().filename().string());
			if (file != f.files.end()) {
				if ((*file).second != time.st_mtime) {
					LOG("File %s changed", (*file).first.c_str());
					f.files[entry.path().filename().string()] = time.st_mtime;
				}
			}
			else {
				LOG("New file %s found", entry.path().filename().string().c_str());
				f.files[entry.path().filename().string()] = time.st_mtime;
			}
		}
	}
}
