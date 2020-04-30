#pragma once

#include <string>

class Panel
{
	friend class m1GUI;
public:
	Panel(const char* name, bool start_active = true);
	virtual ~Panel();

	virtual void Update() = 0;

private:
	std::string name;
	bool active = true;
};

