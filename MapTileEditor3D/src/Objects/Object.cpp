#include "Objects/Object.h"

#include <stack>

#include "Objects/Components/c1Mesh.h"
#include "Objects/Components/c1Transform.h"
#include "Objects/Components/c1Material.h"
#include "Objects/Components/c1Transform.h"

#include "Tools/System/Logger.h"

#include "ExternalTools/mmgr/mmgr.h"

Object::Object(Object* parent, const char* name) : parent(parent), name(name)
{
	transform = CreateComponent<c1Transform>();

	if (parent != nullptr)
		parent->AddChild(this);
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
	if (!active)
		return;

	std::stack<Object*> stack;
	stack.push(this);

	while (!stack.empty()) {

		Object* o = stack.top(); stack.pop();

		for (auto i = o->components.begin(); i != o->components.end(); ++i) {
			(*i)->Update();
		}

		for (auto i = o->children.begin(); i != o->children.end(); ++i) {
			if ((*i)->active)
				stack.push(*i);
		}
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
	return new Object(this);
}

Component* Object::CreateComponent(Component::Type t)
{
	switch (t)
	{
	case Component::Type::Mesh:
		components.push_back(new c1Mesh(this));
		return components.back();
	case Component::Type::Transform:
			components.push_back(new c1Transform(this));
			return components.back();
	case Component::Type::Material:
		components.push_back(new c1Material(this));
		return components.back();
	default:
		LOGW("Component %i not created, not in swith", t);
		break;
	}
	return nullptr;
}

Component* Object::GetComponent(Component::Type t)
{
	for (auto i : components) {
		if (i->type == t)
			return i;
	}
	return nullptr;
}

const std::vector<Component*>& Object::GetComponents() const
{
	return components;
}

nlohmann::json Object::Parse()
{
	nlohmann::json n;

	n["name"] = name;
	n["active"] = active;

	n["components"] = nlohmann::json::array();
	for (auto i = components.begin(); i != components.end(); ++i) {
		n["components"].push_back((*i)->Parse());
	}

	n["children"] = nlohmann::json::array();
	for (auto i = children.begin(); i != children.end(); ++i) {
		n["children"].push_back((*i)->Parse());
	}

	return n;
}

void Object::Unparse(const nlohmann::json& node)
{
	name = node.value("name", "Object");
	active = node.value("active", true);

	for (auto c : node["components"]) {
		Component* comp = GetComponent(c.value("type", Component::Type::NONE));

		if (comp == nullptr)
			comp = CreateComponent(c.value("type", Component::Type::NONE));

		if (comp != nullptr)
			comp->Unparse(c);
	}

	for (auto i : node["children"]) {
		Object* child = CreateChild();
		child->Unparse(i);
	}
}
