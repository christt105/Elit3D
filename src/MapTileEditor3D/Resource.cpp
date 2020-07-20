#include "Resource.h"

#include "Application.h"
#include "FileSystem.h"

Resource::Resource(Type type, const uint64_t& uid) : type(type), uid(uid)
{
}

Resource::~Resource()
{
}

Uint64 Resource::GetUID() const
{
	return uid;
}

void Resource::GenerateFiles()
{
	App->file_system->CopyTo(assets_path.c_str(), library_path.c_str());
}

void Resource::Attach()
{
	if (++references == 1u)
		Load();
}

void Resource::Detach()
{
	if (--references == 0u)
		Unload();
}

Resource::Type Resource::GetType()
{
	return type;
}

std::string Resource::GetStrType()
{
	switch (type)
	{
	case Resource::Type::NONE:
		return std::string("None");
		break;
	case Resource::Type::Mesh:
		return std::string("Mesh");
		break;
	case Resource::Type::Model:
		return std::string("Model");
		break;
	case Resource::Type::Texture:
		return std::string("Texture");
		break;
	case Resource::Type::Tileset:
		return std::string("Tileset");
		break;
	}
	return std::string("None");
}
