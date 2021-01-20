#pragma once

#include "ExternalTools/JSON/json.hpp"

class Object;

class Component
{
public:
	enum class Type {
		NONE = -1,
		Mesh,
		Transform,
		Material,

		MAX
	};

public:
	Component(Object* obj, Type type);
	virtual ~Component();

public:
	virtual void Update() {}

	virtual void OnInspector() {}

	virtual nlohmann::json Parse();

public:
	Object* object = nullptr;

private:
	Type type = Type::NONE;
};

