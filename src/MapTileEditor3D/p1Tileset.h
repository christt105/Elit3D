#pragma once
#include "Panel.h"

class r1Shader;
class r1Texture;
class r1Tileset;

struct TileSetData {
	char buf_name[25];
	bool transparent = false;
	float transparent_color[3];
	int tile_size[4]; // width | height | margin | spacing
	uint64_t imageUID = 0ULL;

	void Reset() {
		tile_size[0] = tile_size[1] = 32;
		tile_size[2] = tile_size[3] = 0;
		transparent_color[0] = transparent_color[1] = transparent_color[2] = 1.f;
		memset(buf_name, 0, 25);
		imageUID = 0ULL;
	}
};

class p1Tileset :
	public Panel
{
public:
	p1Tileset(bool start_enabled = true, bool appear_mainmenubar = true, bool can_close = true);
	~p1Tileset();

	void Start() override;
	void Update() override;

	void DisplayImage(r1Texture* texture, r1Tileset* tile);

	void TileSetInfo(r1Tileset* tile);

	void SelectTex();
	void DeselectTex();

	void SelectTransparentColor(r1Shader*& shader);

	int2 GetTileSelected() const;

private:
	void ModalCreateTileset(bool& modal);
	void ModalSelectImageTileset();

private:
	uint64_t tileset = 0ULL;

	TileSetData data;

	int tile_selected[2] = { -1, -1 };

	bool select_tileset = false;
};

