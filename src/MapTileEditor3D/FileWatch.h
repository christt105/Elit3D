#pragma once

#include "FileSystem.h"
#include <future>

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
	void CheckFolder(const char* folder, Folder& f);

private:
	const char* folder;
	std::future<void> fut;
	bool watch = true;
	Folder root;
};

