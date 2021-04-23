#pragma once
#include "Tools/Map/MapLayer.h"

class MapLayerTile :
    public MapLayer
{
public:
    MapLayerTile(r1Map* map);
    ~MapLayerTile() override;

    void Draw(const int2& size, int tile_width, int tile_height) const override;
    void Reset(const int2& size) override;
    void Resize(const int2& oldSize, const int2& newSize) override;

    void SelectTex() const;

    aiNode* Parse(std::vector<aiMesh*>& meshes) const override;

    void Unparse(const pugi::xml_node& node) override;
    void Unparse(const nlohmann::json& node) override;

public:
    unsigned int id_tex = 0u;
};

