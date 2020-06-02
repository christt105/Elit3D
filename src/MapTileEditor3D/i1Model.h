#pragma once
#include "Importer.h"
class i1Model :
	public Importer
{
public:
	i1Model();
	~i1Model();

	void Import(const char* path) override;
};

