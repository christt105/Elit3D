#pragma once
#include "Module.h"
#include "Resource.h"
#include <SDL_stdinc.h>

struct Folder;

#define ASSETS_PATH "Assets/"
#define ASSETS_TEXTURES_PATH "Assets/Textures/"
#define ASSETS_MODELS_PATH "Assets/Models/"

#define LIBRARY_PATH "Library/"
#define LIBRARY_TEXTURES_PATH "Library/Textures/"
#define LIBRARY_MODELS_PATH "Library/Models/"
#define LIBRARY_MESHES_PATH "Library/Meshes/"

class m1Resources :
	public Module
{
public:
	m1Resources(bool start_enabled = true);
	~m1Resources();

	bool Start() override;
	bool CleanUp() override;

public:
	Uint64 Find(const char* file);
	Resource* Get(const Uint64& uid) const;

	Resource* CreateResource(Resource::Type type, const char* assets_path, const uint64_t& force_uid = 0ULL);

	uint64_t GenerateMeta(const char* file);
private:
	void GenerateLibrary();
	void ImportFiles(const Folder& parent, Resource::Type type = Resource::Type::NONE);

	const char* GetLibraryFromType(Resource::Type type);
	const char* GetExtensionFromType(Resource::Type type);

	void DeleteFromLibrary(Resource::Type type, const uint64_t& meta);

private:
	std::map<uint64_t, Resource*> resources;
};

