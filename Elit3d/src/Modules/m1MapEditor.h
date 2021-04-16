#pragma once

#include "Modules/Base/Module.h"

#include "Tools/Math/int2.h"
#include "ExternalTools/MathGeoLib/include/Math/float3.h"
#include "Tools/Map/MapLayer.h"

class Object;
class p1Scene;
class p1Tileset;
class p1Layers;
class p1Tools;

class MapLayerObject;
class MapLayerTerrain;
class MapLayerTile;

class m1MapEditor :
    public Module
{
public:
    enum class MapTypeExport {
        NONE = -1,

        XML,
        JSON,
        OBJ,

        MAX
    };

    m1MapEditor();
    ~m1MapEditor();

    bool Start() override;
    UpdateStatus Update() override;
    bool CleanUp() override;

    void SaveMap() const;
    void SaveImageMap() const;
    void LoadMap(const uint64_t& id);
    void ReLoadMap();

    void Mouse(const Ray& ray);
    void MouseTileTerrain(r1Map* m, const int2& tile, MapLayerTerrain* layer);
    void MouseTileObject(MapLayerObject* layer, const int2& tile, const int2& mapSize);
    void MouseTile(r1Map* m, MapLayerTile* layer, const int2& tile);
    void ResizeMap(int width, int height);
    int2 GetMapSize() const;

    void ReorderLayers() const;

    MapLayer* AddLayer(MapLayer::Type t);
    void EraseLayer(int index);

    bool ValidMap() const;
    r1Map* GetMap() const;
    bool GetLayers(std::vector<MapLayer*>* &vec) const;

    MapLayerObject* GetObjectLayer(bool create_if_no_exist, bool select);

    void ExportMap(MapTypeExport t, MapLayer::DataTypeExport d) const;

private:
    uint64_t map = 0ULL;
    
    p1Tileset*  panel_tileset = nullptr;
    p1Layers*   panel_layers = nullptr;
    p1Tools*    panel_tools = nullptr;

    int tilesetIndex = -1;
};

