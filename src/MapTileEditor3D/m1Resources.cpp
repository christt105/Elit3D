#include "m1Resources.h"
#include "Application.h"

#include "r1Texture.h"
#include "r1Model.h"
#include "r1Mesh.h"

#include "FileSystem.h"
#include "Random.h"

#include "Logger.h"

#include "ExternalTools/mmgr/mmgr.h"

m1Resources::m1Resources(bool start_enabled) : Module("Resources", start_enabled)
{
}

m1Resources::~m1Resources()
{
}

bool m1Resources::Start()
{
	if (!App->file_system->Exists(LIBRARY_PATH))
		App->file_system->CreateFolder(LIBRARY_PATH);

	if (!App->file_system->Exists(LIBRARY_TEXTURES_PATH))
		App->file_system->CreateFolder(LIBRARY_TEXTURES_PATH);

	if (!App->file_system->Exists(LIBRARY_MODELS_PATH))
		App->file_system->CreateFolder(LIBRARY_MODELS_PATH);

	if (!App->file_system->Exists(LIBRARY_MESHES_PATH))
		App->file_system->CreateFolder(LIBRARY_MESHES_PATH);

	GenerateLibrary();

	return true;
}

bool m1Resources::CleanUp()
{
	for (auto i = resources.begin(); i != resources.end(); ++i) {
		delete (*i).second;
	}
	resources.clear();

	return true;
}

Uint64 m1Resources::Find(const char* file)
{
	for (auto i = resources.begin(); i != resources.end(); ++i) {
		if ((*i).second->name.compare(file) == 0)
			return (*i).first;
	}

	LOGW("Resource %s not found", file);

	return 0ull;
}

Resource* m1Resources::Get(const Uint64& uid) const
{
	auto ret = resources.find(uid);
	return (ret == resources.end()) ? nullptr : (*ret).second;
}

Resource* m1Resources::CreateResource(Resource::Type type, const char* assets_path, const uint64_t& force_uid)
{
	Resource* ret = nullptr;

	switch (type)
	{
	case Resource::Type::Mesh: ret = new r1Mesh((force_uid == 0) ? App->random->RandomGUID() : force_uid);	break;
	case Resource::Type::Model:	ret = new r1Model((force_uid == 0) ? App->random->RandomGUID() : force_uid);	break;
	case Resource::Type::Texture: ret = new r1Texture((force_uid == 0) ? App->random->RandomGUID() : force_uid);	break;
	case Resource::Type::MAX:
		break;
	default:
		break;
	}

	if (ret != nullptr) {
		ret->assets_path.assign(assets_path);
		ret->name = App->file_system->GetNameFile(assets_path);
		ret->extension.assign(App->file_system->GetFileExtension(assets_path));
		ret->library_path.assign(GetLibraryFromType(type) + std::to_string(ret->GetUID()) + GetExtensionFromType(type));

		resources[ret->GetUID()] = ret;
	}

	return ret;
}

void m1Resources::GenerateLibrary()
{
	ImportFiles(App->file_system->GetFilesRecursive("Assets/Tilesets/"), Resource::Type::Texture);
	ImportFiles(App->file_system->GetFilesRecursive("Assets/Models/"), Resource::Type::Model);
}

void m1Resources::ImportFiles(const Folder& parent, Resource::Type type)
{
	for (auto dir = parent.folders.begin(); dir != parent.folders.end(); ++dir) {
		ImportFiles(*dir);
	}
	for (auto file = parent.files.begin(); file != parent.files.end(); ++file) {
		if (App->file_system->GetFileExtension((*file).c_str()).compare("meta") != 0) { // if is not a meta file
			if (App->file_system->Exists((parent.name + *file + ".meta").c_str())) {		// exist meta?
				auto meta = App->file_system->OpenJSONFile((parent.name + *file + ".meta").c_str());
				if (meta.value("timestamp", 0ULL) == App->file_system->LastTimeWrite((parent.name + *file).c_str())) {	// is the same file timestamp?
					if (App->file_system->Exists((GetLibraryFromType(type) + std::to_string(meta.value("UID", 0ULL)) + GetExtensionFromType(type)).c_str())) {	// exists in Library?
						CreateResource(type, (parent.name + *file).c_str(), meta.value("UID", 0ULL));													//Yes, create resource
					}
					else {
						Resource* res = CreateResource(type, (parent.name + *file).c_str(), meta.value("UID", 0ULL));									//No, import and create resource

						res->GenerateFiles();
					}
				}
				else {																									// Delete from library, update timestamp and import
					DeleteFromLibrary(type, meta.value("UID", 0ULL));

					meta["timestamp"] = App->file_system->LastTimeWrite((parent.name + *file).c_str());
					App->file_system->SaveJSONFile((parent.name + *file + ".meta").c_str(), meta);

					Resource* res = CreateResource(type, (parent.name + *file).c_str(), meta.value("UID", 0ULL));

					res->GenerateFiles();
				}
			}
			else {
				uint64_t meta = GenerateMeta((parent.name + *file).c_str());
				
				Resource* res = CreateResource(type, (parent.name + *file).c_str(), meta);

				res->GenerateFiles();
			}
		}
	}
}

uint64_t m1Resources::GenerateMeta(const char* file)
{
	nlohmann::json meta;

	uint64_t uid = App->random->RandomGUID();
	meta["UID"] = uid;
	meta["timestamp"] = App->file_system->LastTimeWrite(file);
	meta["extension"] = App->file_system->GetFileExtension(file);

	App->file_system->SaveJSONFile((file + std::string(".meta")).c_str(), meta);

	return uid;
}

const char* m1Resources::GetLibraryFromType(Resource::Type type)
{
	switch (type)
	{
	case Resource::Type::Mesh:
		return LIBRARY_MESHES_PATH;
	case Resource::Type::Model:
		return LIBRARY_MODELS_PATH;
	case Resource::Type::Texture:
		return LIBRARY_TEXTURES_PATH;
	default:
		break;
	}

	LOGW("No library path found to type %i", (int)type);

	return "";
}

const char* m1Resources::GetExtensionFromType(Resource::Type type)
{
	switch (type)
	{
	case Resource::Type::Mesh:
		return ".mesh";
	case Resource::Type::Model:
		return ".model";
	case Resource::Type::Texture:
		return ".texture";
	default:
		break;
	}

	LOGW("No library path found to type %i", (int)type);

	return "";
}

void m1Resources::DeleteFromLibrary(Resource::Type type, const uint64_t& meta)
{
	switch (type)
	{
	case Resource::Type::Mesh:
		break;
	case Resource::Type::Model:
		break;
	case Resource::Type::Texture:
		App->file_system->DeleteFile((LIBRARY_TEXTURES_PATH + std::to_string(meta) + ".texture").c_str());
		break;
	default:
		break;
	}
}
