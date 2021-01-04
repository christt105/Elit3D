#include "Objects/Object.h"
#include "Objects/Components/c1Mesh.h"
#include "Objects/Components/c1Transform.h"
#include "Objects/Components/c1Material.h"
#include "Objects/Components/c1Transform.h"

#include "ExternalTools/mmgr/mmgr.h"

Object::Object(Object* parent) : parent(parent)
{
	transform = CreateComponent<c1Transform>();
}

Object::~Object()
{
	for (auto i = components.begin(); i != components.end(); ++i)
		delete* i;

	for (auto i = children.begin(); i != children.end(); ++i)
		delete* i;
}

void Object::Update()
{
	for (auto i = components.begin(); i != components.end(); ++i) {
		(*i)->Update();
	}

	for (auto i = children.begin(); i != children.end(); ++i) {
		(*i)->Update();
	}
}

const char* Object::GetName() const
{
	return name.c_str();
}

void Object::SetName(const char* n)
{
	name.assign(n);
}

void Object::AddChild(Object* child)
{
	children.push_back(child);
}

Object* Object::CreateChild()
{
	children.push_back(new Object(this));
	return children.back();
}
