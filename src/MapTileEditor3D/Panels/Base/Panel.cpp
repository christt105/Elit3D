#include "Panels/Base/Panel.h"

#include "Core/Application.h"
#include "Modules/m1Input.h"

Panel::Panel(const char* name, bool start_active, bool appear_mainmenubar, bool can_close, const char* icon)
	: name(name), active(start_active), appear_in_mainmenubar(appear_mainmenubar), can_be_closed(can_close), icon(icon)
{
}

Panel::~Panel()
{
}

void Panel::Start()
{
}

bool Panel::IsFocused() const
{
	return focused;
}

bool Panel::IsOnHover() const
{
	return hover;
}

bool Panel::GetActive() const
{
	return active;
}

void Panel::SetActive(bool act)
{
	active = act;
}

int2 Panel::GetPosition() const
{
	return position;
}

int2 Panel::GetSize() const
{
	return size;
}

int2 Panel::GetMousePosition() const
{	
	int2 window;

	App->input->GetMousePosition(&window.x, &window.y);

	return window - position ;
}
