#pragma once

#include <string>

#include "ExternalTools/ImGui/imgui.h"
#include "ExternalTools/ImGui/IconsFontAwesome5/IconsFontAwesome5.h"

class Panel
{
public:
	friend class m1GUI;
	Panel(const char* name, bool start_active, const char* icon);
	virtual ~Panel();

	virtual void Update() = 0;

public:
	ImGuiWindowFlags flags = ImGuiWindowFlags_None;

private:
	std::string name;
	std::string icon;
	bool active = true;
};

