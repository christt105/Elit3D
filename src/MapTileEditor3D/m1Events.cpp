#include "m1Events.h"
#include "Logger.h"

#include "Application.h"

#include "m1Resources.h"

#include "m1MapEditor.h"
#include "ExternalTools/MathGeoLib/include/Math/float3.h"

#include "Profiler.h"

#include "ExternalTools/mmgr/mmgr.h"

m1Events::m1Events() : Module("Events", true)
{
}

m1Events::~m1Events()
{
}

UpdateStatus m1Events::PreUpdate()
{
	PROFILE_FUNCTION();
	while (!events.empty()) {
		auto e = events.top();
		switch (e->type)
		{
		case Event::Type::FILE_CREATED:
			// Generate metadata and generate library
			break;
		case Event::Type::FILE_MOVED:
			// Check if metadata is within and change resource assets path
			break;
		case Event::Type::FILE_REMOVED:
			// Remove from library and from resources map
			break;
		case Event::Type::FILE_RENAMED:
			//Change metadata name file and resources assets path
			break;
		case Event::Type::FILE_MODIFIED:
			App->resources->ReimportResource(((sTypeVar*)e->info["basic_info"])->value.c_str());
			break;
		case Event::Type::FOLDER_CREATED:
			//Check for resources inside
			break;
		case Event::Type::FOLDER_MOVED:
		case Event::Type::FOLDER_RENAMED:
			// Change resources assets path
			break;
		case Event::Type::FOLDER_REMOVED:
			// delete all resources inside folder
			break;

		case Event::Type::RESIZE_MAP:
			App->map_editor->ResizeMap(((iTypeVar*)e->info["width"])->value, ((iTypeVar*)e->info["height"])->value);
			break;
		case Event::Type::SAVE_MAP:
			App->map_editor->SaveMap();
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

m1Events::Event::Event(): type(m1Events::Event::Type::NONE)
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
