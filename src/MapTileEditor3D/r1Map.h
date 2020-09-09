#pragma once
#include "Resource.h"

#include <map>
#include <vector>

#include "int2.h"
#include "TypeVar.h"
#include "MapLayer.h"

class r1Map :
    public Resource
{
    friend class m1MapEditor;
public:
    r1Map(const uint64_t& _uid);
    ~r1Map();

    void Load() override;
    void Edit(int layer, int row, int col, char r, char g, char b);

    static void CreateNewMap(int width, int height);

private:
    int2 size = { -1, -1 };

    std::map<std::string, TypeVar*> properties;

    std::vector<Layer*> layers;
};

