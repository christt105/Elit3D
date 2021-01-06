#include "Panels/p1Inspector.h"
#include "Core/Application.h"
#include "Modules/m1Objects.h"
#include "Objects/Object.h"
#include "Tools/Map/MapLayer.h"
#include "Modules/m1Resources.h"
#include "Resources/r1Tileset.h"
#include "Resources/r1Map.h"

#include "Tools/FileSystem.h"

p1Inspector::p1Inspector(bool start_enabled, bool appear_mainmenubar, bool can_close)
	: Panel("Inspector", start_enabled, appear_mainmenubar, can_close, ICON_FA_INFO_CIRCLE)
{
}

p1Inspector::~p1Inspector()
{
}

void p1Inspector::Update()
{
	if (selected)
		switch (type)
		{
		case p1Inspector::SelectedType::OBJECT:
			if (App->objects->selected != nullptr) {
				for (auto i = App->objects->selected->components.begin(); i != App->objects->selected->components.end(); ++i) {
					(*i)->OnInspector();
				}
			}
			break;
		case p1Inspector::SelectedType::LAYER:
			((Layer*)selected)->OnInspector();
			break;
		case p1Inspector::SelectedType::EDITOR_MAP:
			((r1Map*)selected)->OnInspector();
			break;
		case p1Inspector::SelectedType::TILESET:
		case p1Inspector::SelectedType::FBX:
		case p1Inspector::SelectedType::MAP:
		case p1Inspector::SelectedType::PNG:
		{
			auto r = App->resources->FindGet(((std::string*)selected)->c_str(), false);
			if (r)
				r->OnInspector();
			break;
		}
		default:
			break;
		}
}

void p1Inspector::SetSelected(void* ptr, SelectedType t)
{
	selected = ptr;
	type = t;
}
