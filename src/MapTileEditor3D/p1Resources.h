#pragma once
#include "Panel.h"
#include "FileSystem.h"

#include "m1Resources.h"
#include "p1Inspector.h"

class p1Project :
	public Panel
{
public:
	p1Project(bool start_enabled = true, bool appear_mainmenubar = true, bool can_close = true);
	~p1Project();

	void Start() override;
	void Update() override;

private:
	void SideTreeFolder(const Folder* folder);

private:
	m1Resources::EResourceType GetEType(const std::string& file) const; //TODO inline
	p1Inspector::SelectedType GetInspectorType(const std::string& file) const; //TODO inline

private:
	Folder* root = nullptr;
	Folder* selected = nullptr;

	std::map<m1Resources::EResourceType, unsigned int> ids;

	float size = 0.5f;
};

