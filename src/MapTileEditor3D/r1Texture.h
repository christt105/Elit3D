#pragma once
#include "Resource.h"

struct aiTexture;

class r1Texture :
	public Resource
{
public:
	r1Texture(const uint64_t& uid);
	~r1Texture();

	void Load() override;
	void Unload() override;

	unsigned int GetBufferID();

	void GenerateFiles(const aiTexture* texture);

private:
	unsigned int id = 0U;

	int width = 0U;
	int height = 0U;
	int channels = 0U;
};
