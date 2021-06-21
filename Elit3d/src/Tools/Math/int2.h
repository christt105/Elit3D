#pragma once

#include <string>

struct ImVec2;

class int2
{
public:
	int2();
	int2(int val);
	int2(int x, int y);
	~int2();

	void Set(int val);
	void Set(int x, int y);
	void Set(const ImVec2& vec);

	int* ptr();
	const int* ptr() const;

	std::string ToString() const;

	int2 operator+(const int2& c) const;
	int2 operator-(const int2& c) const;
	int2 operator*(const int2& c) const;

public:
	int x = 0;
	int y = 0;
};

