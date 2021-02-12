#pragma once
#include "Modules/Base/Module.h"

class Object;

class m1Objects :
	public Module
{
	friend class p1Layers;
	friend class p1Objects;
public:
	m1Objects(bool start_enabled = true);
	~m1Objects();

	Object* CreateEmptyObject(Object* parent = nullptr, const char* name = "Object");
	void DeleteObject(Object* obj);

	bool Start() override;
	UpdateStatus Update() override;

	const Object* GetSelected() const;
	void Unselect();

private:
	Object* selected = nullptr;
	Object* layer_root_selected = nullptr;
};

