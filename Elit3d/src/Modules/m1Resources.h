#pragma once
#include "Modules/Base/Module.h"
#include "Resources/Base/Resource.h"
#include "Tools/Math/Random.h"

struct Folder;
class FileWatch;
class r1Texture;

#define ASSETS_PATH "Assets/"
#define ASSETS_TEXTURES_PATH "Assets/Textures/"
#define ASSETS_MODELS_PATH "Assets/Models/"
#define ASSETS_TILESETS_PATH "Assets/Tilesets/"
#define ASSETS_MAPS_PATH "Assets/Maps/"

class m1Resources :
	public Module
{
public:
	enum class EResourceType {
		NONE = -1,

		//3D models
		TILE,

		//Textures
		RESOURCE_MIN,
		FOLDER,
		PNG,
		FBX,
		MAP,
		TILESET,
		UNKNOWN,
		RESOURCE_MAX,

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
	inline Resource* Get(const Uint64& uid) const {
		auto ret = resources.find(uid);
		return (ret == resources.end()) ? nullptr : (*ret).second;
	}
	Resource* FindGet(const char* file, bool by_name = true);
	Resource* EGet(EResourceType type) const;

	const std::map<uint64_t, Resource*> GetResources() const;

	FileWatch* GetFileWatcher() const;
	void PauseFileWatcher(bool pause);

	template<class T>
	T* CreateResource(const char* assets_path, const uint64_t& force_uid = 0ULL, bool set_strings = true);
	Resource* CreateResource(Resource::Type type, const char* assets_path, const uint64_t& force_uid = 0ULL);
	Resource* NewResource(const char* assets_path);

	void DeleteResource(const uint64_t& uid);

	void SetResourceStrings(Resource* ret, const char* assets_path);

	uint64_t GenerateMeta(const char* file);

	void ReimportResource(const char* file);

	std::vector<Resource*> GetVectorOf(Resource::Type type);
	std::vector<r1Texture*> GetVectorOfTextures() const;

private:
	void GenerateLibrary();
	void LoadEngineResources();
	void ImportFiles(const Folder* parent);

	void StartFileWatcher();
	Resource::Type GetTypeFromStr(const char* type) const;

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
