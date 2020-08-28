#include "int2.h"

int2::int2()
{
}

int2::int2(int x, int y)
{
	this->x = x;
	this->y = y;
}

int2::~int2()
{
}

int* int2::ptr()
{
	return &x;
}

const int* int2::ptr() const
{
	return &x;
}
