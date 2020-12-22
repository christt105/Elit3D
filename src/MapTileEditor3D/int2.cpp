#include "int2.h"

#include "ExternalTools/ImGui/imgui.h"

int2::int2()
{
}

int2::int2(int val)
	: x(val), y(val)
{
}

int2::int2(int x, int y)
	: x(x), y(y)
{
}

int2::~int2()
{
}

void int2::Set(int val)
{
	x = y = val;
}

void int2::Set(int x, int y)
{
	this->x = x;
	this->y = y;
}

void int2::Set(const ImVec2& vec)
{
	x = (int)vec.x;
	y = (int)vec.y;
}

int* int2::ptr()
{
	return &x;
}

const int* int2::ptr() const
{
	return &x;
}

std::string int2::ToString() const
{
	return std::string("(" + std::to_string(x) + ", " + std::to_string(y) + ")");
}

int2 int2::operator+(const int2& c) const
{
	return int2(x - c.x, y - c.y);
}

int2 int2::operator-(const int2& c) const
{
	return int2(x - c.x, y - c.y);
}
