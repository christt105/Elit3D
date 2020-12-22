#pragma once

#include <string>

#include "int2.h"

#include "ExternalTools/ImGui/imgui.h"
#include "ExternalTools/ImGui/IconsFontAwesome5/IconsFontAwesome5.h"
#include "ExternalTools/ImGui/IconsFontAwesome5/IconsFontAwesome5Brands.h"

#include "ExternalTools/JSON/json.hpp"

class Panel
{
	friend class m1GUI;
public:
	Panel(const char* name, bool start_active, bool appear_mainmenubar, bool can_close, const char* icon);
	virtual ~Panel();

	virtual void Start();
	virtual void Update() = 0;

	bool IsFocused() const;
	bool IsOnHover() const;

	bool GetActive() const;
	void SetActive(bool act);

	int2 GetPosition() const;
	int2 GetSize() const;
	int2 GetMousePosition() const;

	virtual void Save(nlohmann::json& node) {}

public:
	ImGuiWindowFlags flags = ImGuiWindowFlags_None;

protected:
	int2 position;
	int2 size;

private:
	std::string name;
	std::string icon;
	bool active = true;
	bool focused = false;
	bool hover = false;
	bool appear_in_mainmenubar = true;
	bool can_be_closed = true;
};

