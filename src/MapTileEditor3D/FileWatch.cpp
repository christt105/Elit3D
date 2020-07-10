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
	root = App->file_system->GetFilesRecursive(folder.c_str());
	
	fut = std::async(std::launch::async, &FileWatch::Watch, this);
#endif
}

void FileWatch::Watch()
{
	while (watch) {
		std::list<m1Events::Event*> events;
		CheckFolder(root, events);
		HandleEvents(events);
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

void FileWatch::CheckFolder(Folder& f, std::list<m1Events::Event*>& ev)
{
	auto i = f.files.begin();
	while (i != f.files.end()) {
		if (!FileSystem::Exists((f.full_path + (*i).first).c_str())) {
			ev.push_back(new m1Events::Event(m1Events::Event::Type::FILE_REMOVED, (f.full_path + (*i).first).c_str()));
			i = f.files.erase(i);
		}
		else
			++i;
	}

	auto j = f.folders.begin();
	while (j != f.folders.end()) {
		if (!FileSystem::Exists((*j).full_path.c_str())) {
			LOG("Folder %s deleted", (*j).full_path.c_str());
			j = f.folders.erase(j);
		}
		else
			++j;
	}

	for (const auto& entry : fs::directory_iterator(f.full_path)) {
		if (entry.is_directory()) {
			auto it = std::find(f.folders.begin(), f.folders.end(), FileSystem::NormalizePath(entry.path().u8string().c_str()) + "/");
			if (it != f.folders.end()) {
				CheckFolder(*it, ev);
			}
			else {
				LOG("New folder %s found", entry.path().u8string().c_str());
				f.folders.push_back(Folder((FileSystem::NormalizePath(entry.path().u8string().c_str()) + "/").c_str()));
			}
		}
		else {
			uint64_t time = FileSystem::LastTimeWrite(entry.path().u8string().c_str());
			auto file = f.files.find(entry.path().filename().string());
			if (file != f.files.end()) {
				if ((*file).second != time) {
					App->events->AddEvent(new m1Events::Event(m1Events::Event::Type::FILE_MODIFIED, (f.full_path + (*file).first).c_str()));
					f.files[entry.path().filename().string()] = time;
				}
			}
			else {
				ev.push_back(new m1Events::Event(m1Events::Event::Type::FILE_CREATED, entry.path().u8string().c_str()));
				f.files[entry.path().filename().string()] = time;
			}
		}
	}
}

void FileWatch::HandleEvents(std::list<m1Events::Event*>& e)
{
	for (auto i = e.begin(); i != e.end(); ++i) {
		if (*i != nullptr) {
			switch ((*i)->type)
			{
			case m1Events::Event::Type::FILE_CREATED:
			case m1Events::Event::Type::FILE_REMOVED:
				CheckIfFileMoved(e, *i, (*i)->type);
				break;
			default:
				LOG("Event with type %i not handled", (*i)->type);
				delete* i;
				break;
			}
		}
	}
}

void FileWatch::CheckIfFileMoved(std::list<m1Events::Event*>& evs, m1Events::Event* e, m1Events::Event::Type type)
{
	bool is_moved = false;
	for (auto j = evs.begin(); j != evs.end(); ++j) {
		if ((*j)->type == (type == m1Events::Event::Type::FILE_CREATED) ? m1Events::Event::Type::FILE_REMOVED : m1Events::Event::Type::FILE_CREATED) {
			//check if is the same name
			if (App->file_system->GetNameFile(
				dynamic_cast<sTypeVar*>(e->info["basic_info"])->value.c_str(), true)
				.compare(App->file_system->GetNameFile(dynamic_cast<sTypeVar*>((*j)->info["basic_info"])->value.c_str(), true)) == 0)
			{
				//File Moved to other folder
				m1Events::Event* ev = new m1Events::Event(m1Events::Event::Type::FILE_MOVED);
				ev->info[(type == m1Events::Event::Type::FILE_CREATED) ? "from" : "to"] = new sTypeVar(dynamic_cast<sTypeVar*>(e->info["basic_info"])->value.c_str());
				ev->info[(type == m1Events::Event::Type::FILE_CREATED) ? "to" : "from"] = new sTypeVar(dynamic_cast<sTypeVar*>((*j)->info["basic_info"])->value.c_str());

				LOG("File %s moved from %s to %s", App->file_system->GetNameFile(dynamic_cast<sTypeVar*>(e->info["basic_info"])->value.c_str(), true).c_str(), ev->info["from"])

				App->events->AddEvent(ev);

				is_moved = true;

				//TODO: delete events

				delete  e;
				delete* j;
				*j = nullptr;

				break;
			}
			else if (/*same folder?*/true) {

			}
			else {
				//is strange to have on the same frame a file created and other deleted
			}
		}
	}

	if (!is_moved) {
		LOG("File %s has created", dynamic_cast<sTypeVar*>(e->info["basic_info"])->value.c_str());
		App->events->AddEvent(e);
	}
}
