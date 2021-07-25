#pragma once

#include "Tools/FileSystem.h"
#include <thread>
#include <mutex>
#include "ExternalTools/efsw/efsw.hpp"

#include "Modules/m1Events.h"

class FileWatch
{
	friend class m1Resources;

	class Listener : public efsw::FileWatchListener {
	public:
		Listener() = default;

		void handleFileAction(efsw::WatchID watchid, const std::string& dir, const std::string& filename, efsw::Action action, std::string oldFilename = "") override;
	};
public:
	FileWatch();
	~FileWatch();

private:
	efsw::FileWatcher* fileWatcher = nullptr;

	Listener* listener = nullptr;
	efsw::WatchID assetsID = 0L;
};
