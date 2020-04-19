#pragma once

#include <string>

enum UpdateStatus {
	UPDATE_CONTINUE,
	UPDATE_STOP,
	UPDATE_ERROR
};

class Module {
public:
	Module(const char* n, bool start_enabled = true);
	virtual ~Module();

	virtual bool Init();
	virtual bool Start();
	virtual UpdateStatus PreUpdate();
	virtual UpdateStatus Update();
	virtual UpdateStatus PostUpdate();
	virtual bool CleanUp();

public:
	std::string name;

private:
	bool enabled = true;
};
