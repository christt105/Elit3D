#pragma once
#include "Modules/Base/Module.h"

class i1Model;

class m1Importer :
	public Module
{
public:
	m1Importer(bool start_enabled = true);
	~m1Importer();

	void Import(const char* path);

private:
	i1Model* model = nullptr;
};

