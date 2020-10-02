#pragma once

#include "FileSystem.h"
#include <future>

#include "m1Events.h"

class FileWatch
{
public:
	FileWatch();
	~FileWatch();

public:
	void Subscribe(const char* folder, bool recursive = true);
	void StartWatching();
	void Watch();

private:
	void CheckFolder(Folder* f, std::list<m1Events::Event*>& ev);
	void HandleEvents(std::list<m1Events::Event*>& e);

	void CheckIfFileMoved(std::list<m1Events::Event*>& evs, m1Events::Event* e, m1Events::Event::Type type);

private:
	std::string folder = "Assets/";
	std::future<void> fut;
	bool watch = true;
	Folder* root = nullptr;

	std::mutex mtx;
};

