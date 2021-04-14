#include "r1Tileset3d.h"
#include "Tools/FileSystem.h"
#include "Core/Application.h"
#include "Modules/m1Resources.h"
#include "Resources/r1Model.h"

#include "ExternalTools/mmgr/mmgr.h"

r1Tileset3d::r1Tileset3d(const uint64_t& uid) : Resource(Resource::Type::Tileset3d, uid)
{
}

r1Tileset3d::~r1Tileset3d()
{
	for (auto& i : tiles)
		if (i != nullptr)
			delete i;
}

void r1Tileset3d::GenerateFiles()
{
	nlohmann::json json;

	for (auto& i : tiles) {
		nlohmann::json tile;

		tile["name"] = i->name;
		tile["transformation"] = i->transform.Parse();
		tile["uid"] = i->uidObject;

		json.push_back(tile);
	}

	FileSystem::SaveJSONFile(path.c_str(), json);
}

void r1Tileset3d::Load()
{
	for (auto& i : tiles) {
		App->resources->Get(i->uidObject)->Attach();
	}
}

void r1Tileset3d::LoadVars()
{
	auto json = FileSystem::OpenJSONFile(path.c_str());
	if (json.is_null())
		return;

	for (auto& i : json) {
		auto t = new Tile3d(i.value("uid", 0ULL), i.value("name", "Unknown"));
		t->transform.Unparse(i["transformation"]);

		tiles.push_back(t);
	}
}

r1Tileset3d::Tile3d::Tile3d(const uint64_t& uid, const std::string& str) : uidObject(uid), name(str) 
{
}
