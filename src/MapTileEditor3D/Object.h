#pragma once

#include <vector>
#include <string>
#include "Component.h"

class Object
{
public:
	Object();
	~Object();

public:
	void Update();

	const char* GetName() const;
	Component* CreateComponent(Component::Type type);

private:
	std::vector<Component*> components;

	std::string name = "Object";
};

