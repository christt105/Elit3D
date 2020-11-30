#pragma once

#include <unordered_map>

#include "int2.h"
#include "Buffer.h"

#include "Globals.h"

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
    Layer();
    ~Layer();

    float height = 0.f;

    unsigned int id_tex = 0u;

    void Draw(const int2& size, int tile_width, int tile_height) const;
    void Reset(const int2& size);

    static void SelectBuffers();
    static void DrawTile(const int2& size);
    static bool HeightOrder(const Layer* l1, const Layer* l2);

    void OnInspector();

    const char* GetName() const;
    void SetName(const char* n);

private:
    void CreateProperty();
    void DisplayProperties();

private:
    static OpenGLBuffers tile;

    TILE_DATA_TYPE* tile_data = nullptr; //TODO: Research about set id with short or int

    std::unordered_map<std::string, TypeVar*> properties;

    std::string name = "Layer";
    char buf[30];
    bool visible = true;
    bool locked = false;
    float opacity = 1.f;
    int displacement[2] = { 0,0 };
};
