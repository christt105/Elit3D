#pragma once
class int2
{
public:
	int2();
	int2(int x, int y);
	~int2();

	int* ptr();
	const int* ptr() const;


public:
	int x = 0;
	int y = 0;
};

