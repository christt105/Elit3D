#pragma once
class Importer
{
public:
	Importer();
	virtual ~Importer();

	virtual void Import(const char* path) = 0;
};

