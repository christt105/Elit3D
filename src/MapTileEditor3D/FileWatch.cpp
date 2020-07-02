#include "FileWatch.h"
#include "Logger.h"
#include "Application.h"

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
	root = App->file_system->GetFilesRecursive(folder);
	
	fut = std::async(std::launch::async, &FileWatch::Watch, this);
}

void FileWatch::Watch()
{
	while (watch) {
		Check();
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

void FileWatch::Check()
{
	for (const auto& entry : fs::directory_iterator(folder)) {
		if (entry.is_directory()) {
		}
		else {
			if (root.files.find(entry.path().filename().string()) != root.files.end()) {

			}
			else {
				LOG("New file %s found", entry.path().filename().string().c_str());
				struct stat time;
				stat(entry.path().u8string().c_str(), &time);
				root.files[entry.path().filename().string()] = time.st_mtime;
			}
		}
	}
}
