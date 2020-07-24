#include "Panel.h"

Panel::Panel(const char* name, bool start_active, const char* icon) : name(name), active(start_active), icon(icon)
{
}

Panel::~Panel()
{
}

void Panel::Start()
{
}

bool Panel::IsFocused()
{
	return focused;
}

bool Panel::IsOnHover()
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
