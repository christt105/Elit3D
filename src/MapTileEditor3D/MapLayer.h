#pragma once
#include "int2.h"
#include "Buffer.h"

class r1Mesh;

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

    const char* GetName() const;
    void SetName(const char* n);

private:
    static OpenGLBuffers tile;

    unsigned char* tile_data = nullptr;

    std::string name = "Layer";
};
