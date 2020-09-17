#include "r1Tileset.h"
#include "Application.h"
#include "m1Resources.h"
#include "FileSystem.h"
#include "r1Texture.h"

#include "Logger.h"

r1Tileset::r1Tileset(const uint64_t& _uid) : Resource(Resource::Type::Tileset, _uid)
{
}

r1Tileset::~r1Tileset()
{
}

void r1Tileset::Load()
{
	auto json = FileSystem::OpenJSONFile(library_path.c_str());

	texture_uid = json.value("Image", 0ULL);
	r1Texture* res = (r1Texture*)App->resources->Get(texture_uid);
	if (res != nullptr)
		res->Attach();
	else
		LOGW("Texture with id %s not found in tileset %s(%s)", std::to_string(texture_uid).c_str(), name.c_str(), std::to_string(uid).c_str());

	ntiles = json.value("ntiles", 0);
	columns = json.value("columns", 0);
	if (res->GetWidth() != columns || res->GetHeight() != ntiles / columns) {
		LOGW("TileSet changed ncolumns or rows, may change the result of map");
		columns = res->GetWidth() / 32;
		ntiles = columns * res->GetHeight() / 32;
	}

	width = json["tile"].value("width", 32);
	height = json["tile"].value("hright", 32);
	margin = json["tile"].value("margin", 0);
	spacing = json["tile"].value("spacing", 0);

	use_transparent = json.value("use transparent", false);
	transparent_color[0] = json["transparent color"].value("r", -1.f);
	transparent_color[1] = json["transparent color"].value("g", -1.f);
	transparent_color[2] = json["transparent color"].value("b", -1.f);
}

uint64_t r1Tileset::GetTextureUID()
{
	return texture_uid;
}
