#pragma once

#include <string>
#include <SDL_stdinc.h>

class Resource
{
	friend class m1Resources;
public:
	enum class Type {
		NONE = -1,

		Mesh, 
		Model, 
		Texture,
		Tileset,

		MAX
	};

public:
	Resource(Type type, const uint64_t& uid);
	virtual ~Resource();

	Uint64 GetUID() const;

	virtual void GenerateFiles();

	void Attach();
	void Detach();

	virtual void GenerateFilesLibrary() {}

	Resource::Type GetType();
	std::string GetStrType();
	
public:
	std::string name;
	std::string assets_path;
	std::string library_path;
	std::string extension;

protected:
	virtual void Load() {}
	virtual void Unload() {}

	Uint64 uid = 0ULL;
	Type type = Type::NONE;

	unsigned int references = 0u;
};

