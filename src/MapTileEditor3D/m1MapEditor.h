#pragma once

#include "Module.h"

#include "int2.h"
#include "ExternalTools/MathGeoLib/include/Math/float3.h"

class Object;
class p1Scene;
class p1Tileset;
class Layer;

class m1MapEditor :
    public Module
{
public:
    m1MapEditor();
    ~m1MapEditor();

    bool Start() override;
    UpdateStatus Update() override;
    bool CleanUp() override;

    void SaveMap() const;
    void SaveImageMap() const;
    void LoadMap(const uint64_t& id);
    void ReLoadMap();

    void MousePicking(const float3& position);
    void ResizeMap(int width, int height);
    int2 GetMapSize() const;

    void AddLayer();
    void EraseLayer(int index);

    bool ValidMap() const;
    bool GetLayers(std::vector<Layer*>& vec) const;

private:
    uint64_t map = 0ULL;
    
    p1Tileset* panel_tileset = nullptr;
};

