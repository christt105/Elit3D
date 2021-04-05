#include "r1Tileset3d.h"

r1Tileset3d::r1Tileset3d(const uint64_t& uid) : Resource(Resource::Type::Tileset3d, uid)
{
}

r1Tileset3d::~r1Tileset3d()
{
	for (auto& i : tiles)
		if (i != nullptr)
			delete i;
}

r1Tileset3d::Tile3d::Tile3d(const uint64_t& uid, const std::string& str) : uidObject(uid), name(str) 
{
}
