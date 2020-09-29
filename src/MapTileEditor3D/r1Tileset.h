#pragma once
#include "Resource.h"
class r1Tileset :
	public Resource
{
	friend class p1Tileset;
public:
	r1Tileset(const uint64_t& _uid);
	~r1Tileset();

	void Load() override;
	
	uint64_t GetTextureUID();

	void OnInspector() override;

private:
	uint64_t texture_uid = 0ULL;

	int width = 0;
	int height = 0;
	int margin = 0;
	int spacing = 0;
	int ntiles = 0;
	int columns = 0;

	bool use_transparent = false;
	float transparent_color[3] = { 0.f, 0.f, 0.f };
};

