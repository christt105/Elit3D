#pragma once
#include "Resources/Base/Resource.h"

#include <unordered_map>
#include <vector>

#include "Modules/m1MapEditor.h"

#include "Tools/Math/int2.h"
#include "Tools/TypeVar.h"
#include "Tools/Map/MapLayer.h"
#include "Panels/p1Tools.h"
#include "ExternalTools/JSON/json.hpp"

class MapLayerTile;

class r1Map :
    public Resource
{
    friend class m1MapEditor;
public:
    r1Map(const uint64_t& _uid);
    ~r1Map();

    void Save(const uint64_t& tileset);
    void Export(const uint64_t& tileset, MapLayer::DataTypeExport d, m1MapEditor::MapTypeExport t);
    void ExportJSON(const uint64_t& tileset, MapLayer::DataTypeExport d);
    void ExportXML(const uint64_t& tileset, MapLayer::DataTypeExport d);
    void ExportOBJ() const;
    void SaveInImage();
    void Load() override;
    void Unload() override;
    void Resize(int width, int height);
    void Edit(MapLayerTile* layer, int row, int col, int brushSize, p1Tools::Tools tool, p1Tools::Shape shape, TILE_DATA_TYPE id, unsigned char g, unsigned char b);

    static void CreateNewMap(int width, int height, const char* path);

    void OnInspector() override;

    int2 GetSize() const;
    void GetSize(int& x, int& y) const;
    void GetSize(int2& size) const;

    bool CheckBoundaries(const int2& point, int brushSize, p1Tools::Tools tool, p1Tools::Shape shape) const;

private:
    int2 size = { -1, -1 };

    Properties properties;

    std::vector<MapLayer*> layers;
};

