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

void Resource::Unattach()
{
	if (--references == 0u)
		Unload();
}
