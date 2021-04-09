#pragma once
#include "Tools/Map/MapLayer.h"

class Object;

class MapLayerObject :
    public MapLayer
{
public:
    MapLayerObject();
    ~MapLayerObject() override;

    void Draw(const int2& size, int tile_width, int tile_height) const override;
    void Reset(const int2& size) override;
    void Resize(const int2& oldSize, const int2& newSize) override;

    std::string Parse(int sizeX, int sizeY, DataTypeExport d) const override;
    nlohmann::json Parse(int sizeX, int sizeY) const override;
    void Unparse(int sizeX, int sizeY, const std::string& data) override;

    nlohmann::json  Serialize(const int2& size) const override;
    void            Deserialize(const nlohmann::json& json, const int2& size) override;

public:
    Object* root = nullptr;
    uint64_t* object_tile_data = nullptr; //TODO set object int map_id and bind with tile_data
};

