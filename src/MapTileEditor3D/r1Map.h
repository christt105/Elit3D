#pragma once
#include "Resource.h"

#include <unordered_map>
#include <vector>

#include "int2.h"
#include "TypeVar.h"
#include "MapLayer.h"
#include "ExternalTools/JSON/json.hpp"

class r1Map :
    public Resource
{
    friend class m1MapEditor;
public:
    r1Map(const uint64_t& _uid);
    ~r1Map();

    void Save(const uint64_t& tileset);
    void ExportXML(const uint64_t& tileset);
    void SaveInImage();
    void Load() override;
    void Unload() override;
    void Resize(int width, int height);
    void Edit(int layer, int row, int col, char r, char g, char b);

    static void CreateNewMap(int width, int height, const char* path);

private:
    void LoadLayers(nlohmann::json& file);
    void LoadProperties(const nlohmann::detail::iter_impl<nlohmann::json>& l, Layer* layer);

private:
    int2 size = { -1, -1 };

    std::unordered_map<std::string, TypeVar*> properties;

    std::vector<Layer*> layers;
};

