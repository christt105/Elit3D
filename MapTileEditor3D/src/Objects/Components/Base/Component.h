#pragma once

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

public:
	Object* object = nullptr;

private:
	Type type = Type::NONE;
};

