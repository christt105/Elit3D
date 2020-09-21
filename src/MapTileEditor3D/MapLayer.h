#pragma once
#include "int2.h"
#include "Buffer.h"

class r1Mesh;

class OpenGLBuffers {
public:
    OpenGLBuffers();
    ~OpenGLBuffers();

    void InitData(const int2& size);

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
    int2 size = int2(10, 10);
    float height = 0.f;

    unsigned int id_tex = 0u;

    void Prepare() const;
    void Update() const;
    static void SelectBuffers();
    static void DrawTile(const int2& size);

protected:
    static OpenGLBuffers tile;
    unsigned char* tile_data = nullptr;
};
