#pragma once
#include "Module.h"
#include "Application.h"
#include "Random.h"
#include "Resource.h"
#include <SDL_stdinc.h>
#include <future>
//#include <winnt.h>

struct Folder;
class FileWatch;

#define ASSETS_PATH "Assets/"
#define ASSETS_TEXTURES_PATH "Assets/Textures/"
#define ASSETS_MODELS_PATH "Assets/Models/"
#define ASSETS_TILESETS_PATH "Assets/Tilesets/"
#define ASSETS_MAPS_PATH "Assets/Maps/"

#define LIBRARY_PATH "Library/"
#define LIBRARY_TEXTURES_PATH "Library/Textures/"
#define LIBRARY_MODELS_PATH "Library/Models/"
#define LIBRARY_MESHES_PATH "Library/Meshes/"
#define LIBRARY_TILESETS_PATH "Library/Tilesets/"
#define LIBRARY_MAPS_PATH "Library/Maps/"

class m1Resources :
	public Module
{
public:
	enum class EResourceType {
		NONE = -1,

		TILE,

		MAX
	};

public:
	m1Resources(bool start_enabled = true);
	~m1Resources();

	bool Start() override;
	bool CleanUp() override;

public:
	Uint64 FindByName(const char* name);
	Uint64 FindByPath(const char* assets_path);
	Resource* Get(const Uint64& uid) const;
	Resource* FindGet(const char* file, bool by_name = true);
	Resource* Get(EResourceType type) const;

	const std::map<uint64_t, Resource*> GetResources() const;

	template<class T>
	T* CreateResource(const char* assets_path, const uint64_t& force_uid = 0ULL, bool set_strings = true);
	Resource* CreateResource(Resource::Type type, const char* assets_path, const uint64_t& force_uid = 0ULL);

	void DeleteResource(const uint64_t& uid);

	void SetResourceStrings(Resource* ret, const char* assets_path);

	uint64_t GenerateMeta(const char* file);

	void ReimportResource(const char* file);

	std::vector<Resource*> GetVectorOf(Resource::Type type);

private:
	void GenerateLibrary();
	void GenerateEngineLibrary();
	void ImportFiles(const Folder& parent);

	void StartFileWatcher();

	//without dot
	const char* GetLibraryFromType(const char* type);
	const char* GetLibraryFromType(Resource::Type type);

	std::string GetLibraryExtension(const char* type);
	std::string GetLibraryExtensionFromType(Resource::Type type);
	Resource::Type GetTypeFromStr(const char* type);

	void DeleteFromLibrary(Resource::Type type, const uint64_t& meta);

	void DeleteMeshes(const uint64_t& meta);

private:
	std::map<uint64_t, Resource*> resources;
	std::map<EResourceType, Resource*> engine_resources;

	FileWatch* filewatch = nullptr;
};

template<class T>
inline T* m1Resources::CreateResource(const char* assets_path, const uint64_t& force_uid, bool set_strings)
{
	T* ret = new T((force_uid == 0) ? Random::RandomGUID() : force_uid);

	if (set_strings)
		SetResourceStrings(ret, assets_path);
	else
		SetResourceStrings(ret, "");
	
	resources[ret->uid] = ret;


	return ret;
}
