#include "p1Inspector.h"
#include "Application.h"
#include "m1Objects.h"
#include "Object.h"

p1Inspector::p1Inspector(bool start_enabled) : Panel("Inspector", start_enabled, ICON_FA_INFO_CIRCLE)
{
}

p1Inspector::~p1Inspector()
{
}

void p1Inspector::Update()
{
	Object* selected = App->objects->selected;
	if (selected != nullptr) {
		for (auto i = selected->components.begin(); i != selected->components.end(); ++i) {
			(*i)->OnInspector();
		}
	}
}
