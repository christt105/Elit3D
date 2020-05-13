#pragma once

#include <vector>
#include <string>
#include "Component.h"

class c1Transform;

class Object
{
public:
	Object(Object* parent = nullptr);
	~Object();

public:
	void Update();

	const char* GetName() const;
	void SetName(const char* name);

	void AddChild(Object* child);
	Object* CreateChild();

	template<class C>
	C* CreateComponent();

	c1Transform* transform = nullptr;
private:
	std::vector<Component*> components;
	std::vector<Object*> children;

	std::string name = "Object";

	Object* parent = nullptr;
};

template<class C>
inline C* Object::CreateComponent()
{
	components.push_back(new C(this));
	return (C*)components.back();
}
