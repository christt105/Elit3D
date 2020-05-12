#include "Object.h"
#include "c1Mesh.h"
#include "c1Transform.h"
#include "c1Material.h"

#include "ExternalTools/mmgr/mmgr.h"

Object::Object() 
{
}

Object::~Object()
{
	for (auto i = components.begin(); i != components.end(); ++i)
		delete* i;
}

void Object::Update()
{
	for (auto i = components.begin(); i != components.end(); ++i) {
		(*i)->Update();
	}
}

const char* Object::GetName() const
{
	return name.c_str();
}

Component* Object::CreateComponent(Component::Type type)
{
	Component* c = nullptr;
	switch (type)
	{
	case Component::Type::Mesh:
		components.push_back(c = new c1Mesh(this));
		return c;
	case Component::Type::Transform:
		components.push_back(c = new c1Transform(this));
		return c;
	case Component::Type::Material:
		components.push_back(c = new c1Material(this));
		return c;
	default:
		break;
	}

	return c;
}
