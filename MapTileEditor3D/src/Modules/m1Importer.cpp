#include "Modules/m1Importer.h"
#include "Core/Application.h"
#include "Tools/FileSystem.h"
#include "Tools/Importers/i1Model.h"

#include "ExternalTools/mmgr/mmgr.h"

m1Importer::m1Importer(bool start_enabled) : Module("Importer", start_enabled)
{
	model = new i1Model();
}

m1Importer::~m1Importer()
{
	delete model;
}

void m1Importer::Import(const char* path)
{
	std::string extension = FileSystem::GetFileExtension(path);
	if (extension.compare("fbx") == 0) {
		model->Import(path);
	}
}
