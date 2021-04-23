#pragma once
#include "Tools/Map/MapLayer.h"
class MapLayerTerrain :
    public MapLayer
{
public:
    MapLayerTerrain(r1Map* map);
    ~MapLayerTerrain() override;

    void Draw(const int2& size, int tile_width, int tile_height) const override;
    void Reset(const int2& size) override;

    void Parse(pugi::xml_node& node, MapLayer::DataTypeExport type) const override;
    void Parse(nlohmann::json& node, MapLayer::DataTypeExport type) const override;
    aiNode* Parse(std::vector<aiMesh*>& meshes) const override;

    void Unparse(const pugi::xml_node& node) override;
    void Unparse(const nlohmann::json& node) override;

public:
    Object* root = nullptr;
};

