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
public:
    Layer();
    ~Layer();
    int2 size = int2(10, 10);
    float height = 0.f;

    unsigned int id_tex = 0u;

    void Prepare();
    void Update();
    static void SelectBuffers();

protected:
    static OpenGLBuffers tile;
    unsigned char* data = nullptr;
};
