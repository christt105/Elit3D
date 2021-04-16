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

    std::string     Parse(int sizeX, int sizeY, DataTypeExport d) const override;
    nlohmann::json  Parse(int sizeX, int sizeY) const override;
    aiNode*         Parse(std::vector<aiMesh*>& meshes) const override;
    void Unparse(int sizeX, int sizeY, const std::string& data) override;

    nlohmann::json  Serialize(const int2& size) const override;
    void            Deserialize(const nlohmann::json& json, const int2& size) override;

public:
    TILE_DATA_TYPE* tile_data = nullptr;

    unsigned int id_tex = 0u;
};

