#pragma once
#include "Panel.h"
#include "FileSystem.h"

#include "m1Resources.h"

class p1Resources :
	public Panel
{
public:
	p1Resources(bool start_enabled = true, bool appear_mainmenubar = true, bool can_close = true);
	~p1Resources();

	void Start() override;
	void Update() override;

private:
	m1Resources::EResourceType GetType(const std::string& file) const; //TODO inline

private:
	Folder* root = nullptr;
	Folder* selected = nullptr;
};

