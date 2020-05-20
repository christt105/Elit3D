#pragma once
#include "Module.h"
#include "Application.h"
#include "Random.h"
#include "Resource.h"
#include <SDL_stdinc.h>
//#include <winnt.h>

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

	template<class T>
	T* CreateResource(const char* assets_path, const uint64_t& force_uid = 0ULL);
	Resource* CreateResource(Resource::Type type, const char* assets_path, const uint64_t& force_uid = 0ULL);

	void SetResourceStrings(Resource* ret, const char* assets_path);

	uint64_t GenerateMeta(const char* file);

	std::vector<Resource*> GetVectorOf(Resource::Type type);

private:
	void GenerateLibrary();
	void ImportFiles(const Folder& parent, Resource::Type type = Resource::Type::NONE);

	const char* GetLibraryFromType(Resource::Type type);
	const char* GetExtensionFromType(Resource::Type type);

	void DeleteFromLibrary(Resource::Type type, const uint64_t& meta);

	void DeleteMeshes(const uint64_t& meta);

private:
	std::map<uint64_t, Resource*> resources;
};

template<class T>
inline T* m1Resources::CreateResource(const char* assets_path, const uint64_t& force_uid)
{
	T* ret = new T((force_uid == 0) ? App->random->RandomGUID() : force_uid);

	SetResourceStrings(ret, assets_path);

	resources[ret->GetUID()] = ret;


	return ret;
}
