#pragma once
#include "Panel.h"

struct TileSetData {
	char buf_name[25];
	bool transparent = false;
	float transparent_color[3];
	int tile_size[4]; // width | height | margin | spacing
	uint64_t imageUID = 0ULL;
};

class p1Tileset :
	public Panel
{
public:
	p1Tileset(bool start_enabled = true);
	~p1Tileset();

	void Update() override;

private:
	void ModalCreateTileset(bool& modal);
	void ModalSelectImageTileset();

private:
	uint64_t tileset = 0ULL;

	TileSetData data;

	bool select_tileset = false;
};

