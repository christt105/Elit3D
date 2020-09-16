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

    void Save();
    void Load() override;
    void Unload() override;
    void Resize(int width, int height);
    void Edit(int layer, int row, int col, char r, char g, char b);

    static void CreateNewMap(int width, int height);

    static inline unsigned int GetIndexOf2DArray(unsigned int column, unsigned int row, unsigned int width);
    static inline unsigned int GetIndexOf2DArray(const int2& colrow, unsigned int width);
    static inline int2 GetColRowOf2DArray(unsigned int index, unsigned int width);

private:
    int2 size = { -1, -1 };

    std::map<std::string, TypeVar*> properties;

    std::vector<Layer*> layers;
};

