#pragma once

#include "Panel.h"

class p1Inspector :
	public Panel
{
public:
	enum class SelectedType {
		NONE = -1,

		OBJECT,
		LAYER,

		MAX
	};
public:
	p1Inspector(bool start_enabled = true, bool appear_mainmenubar = true, bool can_close = true);
	~p1Inspector();

	void Update() override;

	void SetSelected(void* ptr, SelectedType type);

private:
	void* selected = nullptr;
	SelectedType type = SelectedType::NONE;
};

