#include "FileWatch.h"
#include "Logger.h"
#include "Application.h"
#include "Globals.h"

#include <filesystem>

#include "Profiler.h"

#include "ExternalTools/mmgr/mmgr.h"

namespace fs = std::filesystem;

FileWatch::FileWatch()
{
}

FileWatch::~FileWatch()
{
	if (watch) {
		watch = false;
		thread.join();
	}
}

void FileWatch::Subscribe(const char* froot)
{
	folder = froot;
}

void FileWatch::StartWatching()
{
	root = FileSystem::GetPtrFolder(folder.c_str());
#if USE_FILEWATCHER
	watch = true;
	thread = std::thread(&FileWatch::Watch, this);
#else
	watch = false;
#endif
}

void FileWatch::Watch()
{
	/*try {*/
		while (watch) {
			while(pause_watch){
				conditional.notify_one();
				std::unique_lock lock(mtx);
				conditional.wait(lock);
			}
			std::list<m1Events::Event*> events;
			CheckFolders(events);
			HandleEvents(events);
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	/*}*/
	/*catch (const std::exception& ex)
	{
		LOGE("FileWatcher thread finished with exception: %s", ex.what());
	}*/
}

void FileWatch::Pause(bool pause)
{
	if (!watch)
		return;
	std::unique_lock<std::mutex> lock(mtx);
	pause_watch = pause;
	if (pause) {
		conditional.wait(lock);
	}
	else {
		create_events = false;
		std::list<m1Events::Event*> dummy;
		CheckFolders(dummy);
		create_events = true;
		conditional.notify_one();
	}
}

void FileWatch::CheckFolders(std::list<m1Events::Event*>& ev)
{
	PROFILE_FUNCTION();

	std::stack<Folder*> stack;
	stack.push(root);

	while (!stack.empty()) {
		Folder* f = stack.top();
		stack.pop();

		CheckRemovedFiles(f, ev);
		CheckRemovedFolders(f, ev);
		CheckFilesCreatedAndRemoved(f, stack, ev);
	}
}

void FileWatch::CheckFilesCreatedAndRemoved(Folder* f, std::stack<Folder*>& stack, std::list<m1Events::Event*>& ev)
{
	//std::unique_lock<std::mutex> lock(mtx);
	for (const auto& entry : fs::directory_iterator(f->full_path)) {
		if (entry.is_directory()) {
			std::vector<Folder*>::iterator it = f->folders.end();
			std::string s = FileSystem::NormalizePath(entry.path().u8string().c_str()) + "/";
			for (auto i = f->folders.begin(); i != f->folders.end(); ++i)
				if ((*i)->full_path.compare(s) == 0) {
					it = i;
					break;
				}
			if (it != f->folders.end()) {
				stack.push(*it);
			}
			else {
				if (create_events)
					ev.push_back(new m1Events::Event(m1Events::Event::Type::FOLDER_CREATED, f->full_path.c_str()));
				f->folders.push_back(new Folder((FileSystem::NormalizePath(entry.path().u8string().c_str()) + "/").c_str(), f));
			}
		}
		else {
			uint64_t time = FileSystem::LastTimeWrite(entry.path().u8string().c_str());
			auto file = f->files.find(entry.path().filename().string());
			if (file != f->files.end()) {
				if ((*file).second != time) {
					if (create_events) {
						App->events->AddEvent(new m1Events::Event(m1Events::Event::Type::FILE_MODIFIED, (f->full_path + (*file).first).c_str()));
					}
					f->files[entry.path().filename().string()] = time;
				}
			}
			else {
				if (create_events)
					ev.push_back(new m1Events::Event(m1Events::Event::Type::FILE_CREATED, entry.path().u8string().c_str()));
				f->files[entry.path().filename().string()] = time;
			}
		}
	}
}

void FileWatch::CheckRemovedFolders(Folder* f, std::list<m1Events::Event*>& ev)
{
	//std::unique_lock<std::mutex> lock(mtx);
	auto j = f->folders.begin();
	while (j != f->folders.end()) {
		if (!FileSystem::Exists((*j)->full_path.c_str())) {
			if (create_events) {
				ev.push_back(new m1Events::Event(m1Events::Event::Type::FOLDER_REMOVED, (*j)->full_path.c_str()));

				std::stack<Folder*> folders;
				folders.push(*j);
				while (!folders.empty()) {
					Folder* itf = folders.top();
					folders.pop();
					for (auto i = (*itf).files.begin(); i != (*itf).files.end(); ++i) {
						if (FileSystem::GetFileExtension((*i).first.c_str()).compare("meta") != 0)
							App->events->AddEvent(new m1Events::Event(m1Events::Event::Type::FILE_REMOVED, (*i).first.c_str()));
					}

					for (auto i = (*itf).folders.begin(); i != (*itf).folders.end(); ++i) {
						folders.push(*i);
					}
				}
			}


			delete* j;
			j = f->folders.erase(j);
		}
		else
			++j;
	}
}

void FileWatch::CheckRemovedFiles(Folder* f, std::list<m1Events::Event*>& ev)
{
	//std::unique_lock<std::mutex> lock(mtx);
	auto i = f->files.begin();
	while (i != f->files.end()) {
		if (!FileSystem::Exists((f->full_path + (*i).first).c_str())) {
			if (create_events)
				ev.push_back(new m1Events::Event(m1Events::Event::Type::FILE_REMOVED, (f->full_path + (*i).first).c_str()));
			i = f->files.erase(i);
		}
		else
			++i;
	}
}

void FileWatch::HandleEvents(std::list<m1Events::Event*>& e)
{
	auto i = e.begin();
	while(i != e.end()) {
		if (*i != nullptr) {
			switch ((*i)->type)
			{
			case m1Events::Event::Type::FILE_CREATED:
			case m1Events::Event::Type::FILE_REMOVED:
				CheckIfFileMoved(e, i, (*i)->type);
				continue;
			case m1Events::Event::Type::FOLDER_CREATED:
			case m1Events::Event::Type::FOLDER_REMOVED:
				CheckIfFolderMoved(e, i, (*i)->type);
				continue;
			default:
				LOG("Event with type %i not handled", (*i)->type);
				delete* i;
				break;
			}
		}
		++i;
	}
}

void FileWatch::CheckIfFileMoved(std::list<m1Events::Event*>& evs, std::list<m1Events::Event*>::iterator& e, m1Events::Event::Type type)
{
	bool only_created = true;
	for (auto j = evs.begin(); j != evs.end(); ++j) {
		if (type == m1Events::Event::Type::FILE_CREATED && (*j)->type == m1Events::Event::Type::FILE_REMOVED ||
			type == m1Events::Event::Type::FILE_REMOVED && (*j)->type == m1Events::Event::Type::FILE_CREATED) {
			//check if is the same name
			std::string s1 = dynamic_cast<sTypeVar*>((*e)->info["basic_info"])->value.c_str();
			std::string s2 = dynamic_cast<sTypeVar*>((*j)->info["basic_info"])->value.c_str();
			if (FileSystem::GetNameFile(
				s1.c_str(), true)
				.compare(FileSystem::GetNameFile(s2.c_str(), true)) == 0)
			{
				//File Moved to other folder
				if (create_events) {
					m1Events::Event* ev = new m1Events::Event(m1Events::Event::Type::FILE_MOVED);
					ev->info[(type == m1Events::Event::Type::FILE_CREATED) ? "from" : "to"] = new sTypeVar(s1);
					ev->info[(type == m1Events::Event::Type::FILE_CREATED) ? "to" : "from"] = new sTypeVar(s2);

					LOG("File %s moved from %s to %s",
						FileSystem::GetNameFile(s1.c_str(), true).c_str(),
						dynamic_cast<sTypeVar*>(ev->info["from"])->value.c_str(),
						dynamic_cast<sTypeVar*>(ev->info["to"])->value.c_str());

					App->events->AddEvent(ev);
				}

				only_created = false;

				delete* e;
				delete* j;
				evs.remove(*e);
				evs.remove(*j);

				e = evs.begin();

				break;
			}
			else if (FileSystem::GetFolder(s1.c_str()).compare(FileSystem::GetFolder(s2.c_str())) == 0) {
				//File renamed
				if (create_events) {
					m1Events::Event* ev = new m1Events::Event(m1Events::Event::Type::FILE_RENAMED);
					ev->info[(type == m1Events::Event::Type::FILE_CREATED) ? "to" : "from"] = new sTypeVar(FileSystem::GetNameFile(s1.c_str(), true));
					ev->info[(type == m1Events::Event::Type::FILE_CREATED) ? "from" : "to"] = new sTypeVar(FileSystem::GetNameFile(s2.c_str(), true));

					LOG("File %s renamed to %s",
						FileSystem::GetNameFile(s1.c_str(), true).c_str(),
						dynamic_cast<sTypeVar*>(ev->info["to"])->value.c_str());

					App->events->AddEvent(ev);
				}

				only_created = false;

				delete* e;
				delete* j;
				evs.remove(*e);
				evs.remove(*j);

				e = evs.begin();

				break;
			}
			else {
				//is strange to have on the same frame a file created and other deleted
				LOGW("File %s created and %s removed", s1.c_str(), s2.c_str());
			}
		}
	}

	if (only_created) {
		LOG("File %s has created", dynamic_cast<sTypeVar*>((*e)->info["basic_info"])->value.c_str());
		App->events->AddEvent(*e);
		++e;
	}
}

void FileWatch::CheckIfFolderMoved(std::list<m1Events::Event*>& evs, std::list<m1Events::Event*>::iterator& e, m1Events::Event::Type type)
{
	bool only_created = true;
	for (auto j = evs.begin(); j != evs.end(); ++j) {
		if (type == m1Events::Event::Type::FOLDER_CREATED && (*j)->type == m1Events::Event::Type::FOLDER_REMOVED ||
			type == m1Events::Event::Type::FOLDER_REMOVED && (*j)->type == m1Events::Event::Type::FOLDER_CREATED) {
			//check if is the same name
			std::string s1 = dynamic_cast<sTypeVar*>((*e)->info["basic_info"])->value.c_str();
			std::string s2 = dynamic_cast<sTypeVar*>((*j)->info["basic_info"])->value.c_str();
			if (FileSystem::GetNameFolder(s1.c_str()).compare(FileSystem::GetNameFolder(s2.c_str())) == 0)
			{
				//Folder moved to other folder
				if (create_events) {
					m1Events::Event* ev = new m1Events::Event(m1Events::Event::Type::FOLDER_MOVED);
					ev->info[(type == m1Events::Event::Type::FOLDER_CREATED) ? "from" : "to"] = new sTypeVar(s1);
					ev->info[(type == m1Events::Event::Type::FOLDER_CREATED) ? "to" : "from"] = new sTypeVar(s2);

					LOG("Folder %s moved from %s to %s",
						FileSystem::GetNameFolder(s1.c_str()).c_str(),
						dynamic_cast<sTypeVar*>(ev->info["from"])->value.c_str(),
						dynamic_cast<sTypeVar*>(ev->info["to"])->value.c_str());

					App->events->AddEvent(ev);
				}

				only_created = false;

				delete* e;
				delete* j;
				evs.remove(*e);
				evs.remove(*j);

				e = evs.begin();

				break;
			}
			else if (FileSystem::GetParentFolder(s1.c_str()).compare(FileSystem::GetParentFolder(s2.c_str())) == 0) {
				//Folder renamed
				if (create_events) {
					m1Events::Event* ev = new m1Events::Event(m1Events::Event::Type::FOLDER_RENAMED);
					ev->info[(type == m1Events::Event::Type::FOLDER_CREATED) ? "to" : "from"] = new sTypeVar(s1.c_str());
					ev->info[(type == m1Events::Event::Type::FOLDER_CREATED) ? "from" : "to"] = new sTypeVar(s2.c_str());

					LOG("Folder %s renamed to %s",
						s1.c_str(),
						dynamic_cast<sTypeVar*>(ev->info["to"])->value.c_str());

					App->events->AddEvent(ev);
				}

				only_created = false;

				delete* e;
				delete* j;
				evs.remove(*e);
				evs.remove(*j);

				e = evs.begin();

				break;
			}
			else {
				//is strange to have on the same frame a file created and other deleted
				LOGW("Folder %s created and %s removed", s1.c_str(), s2.c_str());
			}
		}
	}

	if (only_created) {
		LOG("Folder %s has created", dynamic_cast<sTypeVar*>((*e)->info["basic_info"])->value.c_str());
		App->events->AddEvent(*e);
		++e;
	}
}
