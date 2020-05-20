#pragma once

#include <string>

#include "ExternalTools/ImGui/imgui.h"

class Panel
{
public:
	friend class m1GUI;
	Panel(const char* name, bool start_active = true);
	virtual ~Panel();

	virtual void Update() = 0;

public:
	ImGuiWindowFlags flags = ImGuiWindowFlags_None;

private:
	std::string name;
	bool active = true;
};

