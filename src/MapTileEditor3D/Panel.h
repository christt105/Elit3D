#pragma once

#include <string>

class Panel
{
public:
	friend class m1GUI;
	Panel(const char* name, bool start_active = true);
	virtual ~Panel();

	virtual void Update() = 0;

private:
	std::string name;
	bool active = true;
};

