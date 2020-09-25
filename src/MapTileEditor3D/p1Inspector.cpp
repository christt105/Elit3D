#include "p1Inspector.h"
#include "Application.h"
#include "m1Objects.h"
#include "Object.h"
#include "MapLayer.h"

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
		default:
			break;
		}
}

void p1Inspector::SetSelected(void* ptr, SelectedType t)
{
	selected = ptr;
	type = t;
}
