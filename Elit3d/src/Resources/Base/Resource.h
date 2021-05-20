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
		Object,
		Map,
		Tileset3d,

		MAX
	};

public:
	Resource(Type type, const uint64_t& uid);
	virtual ~Resource();

	Uint64 GetUID() const;

	void Attach();
	void Detach();

	virtual void OnInspector();
	virtual void GenerateFiles() {}
	virtual void UpdateFiles() {}

	inline Resource::Type GetType() const { return type; }
	std::string GetStrType() const;
	
public:
	std::string name;
	std::string path;
	std::string extension;

protected:
	virtual void Load() {}
	virtual void LoadVars() {}
	virtual void Unload() {}

	uint64_t uid = 0ULL;
	Type type = Type::NONE;

	unsigned int references = 0u;
};

