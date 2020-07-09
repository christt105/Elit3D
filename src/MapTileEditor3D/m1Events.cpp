#include "m1Events.h"
#include "Logger.h"

#include "ExternalTools/mmgr/mmgr.h"

m1Events::m1Events() : Module("Events", true)
{
}

m1Events::~m1Events()
{
}

UpdateStatus m1Events::PreUpdate()
{
	while (!events.empty()) {
		auto e = events.top();
		switch (e->type)
		{
		case Event::Type::TEST:
			break;
		case Event::Type::FILE_CREATED:
			break;
		case Event::Type::FILE_MOVED:
			break;
		case Event::Type::FILE_REMOVED:
			break;
		case Event::Type::FILE_RENAMED:
			break;
		default:
			break;
		}
		delete e;
		events.pop();
	}

	return UpdateStatus::UPDATE_CONTINUE;
}

void m1Events::AddEvent(Event* e)
{
	events.push(e);
}

Event::Event(): type(Type::NONE)
{
}

Event::Event(Type t) : type(t)
{
}

Event::~Event()
{
	for (auto i = info.begin(); i != info.end(); ++i) {
		delete (*i).second;
	}
}
