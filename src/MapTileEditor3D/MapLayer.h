#pragma once

#include <unordered_map>

#include "int2.h"
#include "Buffer.h"

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
public:
    Layer();
    ~Layer();

    float height = 0.f;

    unsigned int id_tex = 0u;

    void Prepare() const;
    void Update(const int2& size) const;
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

    unsigned char* tile_data = nullptr;

    std::unordered_map<std::string, TypeVar*> properties;

    std::string name = "Layer";
    char buf[30];
    bool visible = true;
    bool locked = false;
    float opacity = 1.f;
};
