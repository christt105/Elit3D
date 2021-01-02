#pragma once

#include <unordered_map>

#include "int2.h"
#include "Buffer.h"

#include "Globals.h"

#include "Properties.h"

class r1Mesh;
struct TypeVar;

class OpenGLBuffers {
public:
    OpenGLBuffers();
    ~OpenGLBuffers();

    void InitData();

    unsigned int VAO = 0u;

    Buffer<float> vertices;
    Buffer<unsigned int> indices;
    Buffer<float> texture;
};

class Layer {
    friend class r1Map;
    friend class m1MapEditor;
    friend class p1Layers;
public:
    enum class DataTypeExport {
        NONE = -1,

        CSV,
        CSV_NO_NEWLINE,
        BASE64_NO_COMPRESSION,
        BASE64_ZLIB,

        MAX
    };

    Layer();
    ~Layer();

    float height = 0.f;

    unsigned int id_tex = 0u;

    void Draw(const int2& size, int tile_width, int tile_height) const;
    void Reset(const int2& size);
    void SelectTex() const;

    static void SelectBuffers();
    static void DrawTile(const int2& size);
    static bool HeightOrder(const Layer* l1, const Layer* l2);

    void OnInspector();

    std::string Parse(int sizeX, int sizeY, DataTypeExport d) const;
    nlohmann::json Parse(int sizeX, int sizeY) const;
    void Unparse(int sizeX, int sizeY, const std::string& data);

    const char* GetName() const;
    void SetName(const char* n);

private:
    static OpenGLBuffers tile;

    TILE_DATA_TYPE* tile_data = nullptr; //TODO: Research about set id with short or int

    Properties properties;

    std::string name = "Layer";
    char buf[30];
    bool visible = true;
    bool locked = false;
    float opacity = 1.f;
    int displacement[2] = { 0,0 };
};
