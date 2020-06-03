#include "r1Tileset.h"
#include "Application.h"
#include "m1Resources.h"
#include "FileSystem.h"

#include "Logger.h"

r1Tileset::r1Tileset(const uint64_t& _uid) : Resource(Resource::Type::Tileset, _uid)
{
}

r1Tileset::~r1Tileset()
{
}

void r1Tileset::Load()
{
	auto json = App->file_system->OpenJSONFile(library_path.c_str());

	texture_uid = json.value("Image", 0ULL);
	Resource* res = App->resources->Get(texture_uid);
	if (res != nullptr)
		res->Attach();
	else
		LOGW("Texture with id %s not found in tileset %s(%s)", std::to_string(texture_uid).c_str(), name.c_str(), std::to_string(uid).c_str());

	ntiles = json["tile"].value("ntiles", 0);
	columns = json["tile"].value("columns", 0);

	width = json["tile"].value("width", 32);
	height = json["tile"].value("hright", 32);
	margin = json["tile"].value("margin", 0);
	spacing = json["tile"].value("spacing", 0);
}

uint64_t r1Tileset::GetTextureUID()
{
	return texture_uid;
}
