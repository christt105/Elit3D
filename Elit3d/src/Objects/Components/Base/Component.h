#pragma once

#include "ExternalTools/JSON/json.hpp"

class Object;

class Component
{
	friend class Object;
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

	inline Type GetType() const { return type; }

	virtual nlohmann::json	Parse();
	virtual void			Unparse(const nlohmann::json& node) {}

public:
	Object* object = nullptr;

private:
	Type type = Type::NONE;
};

