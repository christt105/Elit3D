#pragma once
#include "Panel.h"

#include "Globals.h"

class r1Shader;
class r1Texture;
class r1Tileset;

class p1Tileset :
	public Panel
{
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
public:
	p1Tileset(bool start_enabled = true, bool appear_mainmenubar = true, bool can_close = true);
	~p1Tileset();

	void Start() override;
	void Update() override;

	void DisplayImage(r1Texture* texture, r1Tileset* tile);

	void TileSetInfo(r1Tileset* tile);

	bool SelectTex();
	void DeselectTex();
	void SelectTileset(const uint64_t& uid);

	void SelectTransparentColor(r1Shader*& shader);
	void SetColumnUniform(r1Shader*& shader);

	int2 GetTileSelected() const;
	TILE_DATA_TYPE GetTileIDSelected() const;
	void SetTileIDSelected(TILE_DATA_TYPE id);
	int GetTileWidth() const;
	int GetTileHeight() const;

	const uint64_t& GetTileset() const;
	int2 GetTilesetSize() const;

private:
	void ModalCreateTileset(bool& modal);
	void ModalSelectImageTileset();

private:
	uint64_t tileset = 0ULL;

	TileSetData data;

	int tile_selected[2] = { -1, -1 };

	bool select_tileset = false;
};

