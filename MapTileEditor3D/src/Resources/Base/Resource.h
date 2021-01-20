#pragma once

#include <string>
#include <SDL_stdinc.h>

class Resource
{
	friend class m1Resources;
	friend class p1Inspector;
	friend class p1DebugResources;
public:
	enum class Type {
		NONE = -1,

		Mesh, 
		Model, 
		Texture,
		Tileset,
		Map,

		MAX
	};

public:
	Resource(Type type, const uint64_t& uid);
	virtual ~Resource();

	Uint64 GetUID() const;

	void Attach();
	void Detach();

	virtual void OnInspector() {} //TODO: 
	virtual void GenerateFiles() {}
	virtual void UpdateFiles() {}

	Resource::Type GetType() const;
	std::string GetStrType() const;
	
public:
	std::string name;
	std::string path;
	std::string extension;

protected:
	virtual void Load() {}
	virtual void LoadVars() {}
	virtual void Unload() {}

	Uint64 uid = 0ULL;
	Type type = Type::NONE;

	unsigned int references = 0u;
};

