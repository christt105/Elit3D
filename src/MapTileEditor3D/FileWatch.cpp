#include "FileWatch.h"
#include "Logger.h"
#include "Application.h"

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
	Folder base = FileSystem::GetFilesRecursive(folder);

	for (auto i = base.files.begin(); i != base.files.end(); ++i) {
		if (root.files.find((*i).first) != root.files.end()) {

		}
		else {
			LOG("New file %s found", (*i).first.c_str());
			root.files[(*i).first] = (*i).second;
		}
	}
}
