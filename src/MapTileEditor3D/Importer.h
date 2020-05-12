#pragma once
class Importer
{
public:
	Importer();
	~Importer();

	virtual void Import(const char* path) = 0;
};

