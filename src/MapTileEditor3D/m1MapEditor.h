#pragma once

#include "Module.h"

#include "int2.h"
#include "ExternalTools/MathGeoLib/include/Math/float3.h"
#include "MapLayer.h"

class Object;
class p1Scene;
class p1Tileset;
class p1Layers;
class p1Tools;

class m1MapEditor :
    public Module
{
public:
    enum class MapTypeExport {
        NONE = -1,

        XML,
        JSON,

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

    void MousePicking(const Ray& ray);
    void ResizeMap(int width, int height);
    int2 GetMapSize() const;

    void AddLayer();
    void EraseLayer(int index);

    bool ValidMap() const;
    r1Map* GetMap() const;
    bool GetLayers(std::vector<Layer*>* &vec) const;

    void ExportMap(MapTypeExport t, Layer::DataTypeExport d) const;

private:
    uint64_t map = 0ULL;
    
    p1Tileset*  panel_tileset = nullptr;
    p1Layers*   panel_layers = nullptr;
    p1Tools*    panel_tools = nullptr;
};

