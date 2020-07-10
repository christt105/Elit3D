#include "m1Events.h"
#include "Logger.h"

#include "ExternalTools/mmgr/mmgr.h"

m1Events::m1Events() : Module("Events", true)
{
	events.push(new Event(Event::Type::FILE_REMOVED, "XD LOLLLLLL"));
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

m1Events::Event::Event(): type(Event::Type::NONE)
{
}

m1Events::Event::Event(Type t, const char* basic_info) : type(t)
{
	if (basic_info != NULL)
		info["basic_info"] = new sTypeVar(basic_info);
}

m1Events::Event::~Event()
{
	for (auto i = info.begin(); i != info.end(); ++i) {
		delete (*i).second;
	}
}
