#pragma once

#include <vector>
#include <string>
#include <typeinfo>
#include "Objects/Components/Base/Component.h"
#include "ExternalTools/JSON/json.hpp"

class c1Transform;

class Object
{
	friend class p1Inspector;
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
	Component* CreateComponent(Component::Type t);
	template<class C>
	C* GetComponent();
	Component* GetComponent(Component::Type t);

	c1Transform* transform = nullptr;

	std::vector<Object*> children;

	Object* parent = nullptr;

	nlohmann::json	Parse();
	void			Unparse(const nlohmann::json& node);

private:
	std::vector<Component*> components;

	std::string name = "Object";
	bool active = true;
};

template<class C>
inline C* Object::CreateComponent()
{
	components.push_back(new C(this));
	return (C*)components.back();
}

template<class C>
inline C* Object::GetComponent()
{
	for (auto i = components.begin(); i != components.end(); ++i) {
		if (typeid(**i) == typeid(C))
			return (C*)*i;
	}
	return NULL;
}
