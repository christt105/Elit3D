#pragma once
#include "Importer.h"
class i1Tileset :
	public Importer
{
public:
	i1Tileset();
	~i1Tileset();

	void Import(const char* path) override;
};
