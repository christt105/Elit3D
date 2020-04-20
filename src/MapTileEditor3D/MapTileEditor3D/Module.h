#pragma once

#include "ExternalTools/JSON/json.hpp"

enum class UpdateStatus {
	UPDATE_CONTINUE,
	UPDATE_STOP,
	UPDATE_ERROR
};

class Module {
public:
	Module(const char* n, bool start_enabled = true);
	virtual ~Module();

	virtual bool Init(const nlohmann::json& node);
	virtual bool Start();
	virtual UpdateStatus PreUpdate();
	virtual UpdateStatus Update();
	virtual UpdateStatus PostUpdate();
	virtual bool CleanUp();

	static std::string UpdateStatusToString(UpdateStatus us);

public:
	std::string name;

private:
	bool enabled = true;
};
