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
public:
    Layer();
    ~Layer();
    int2 position = int2(0, 0);

    int size = 100;

    void Update();
    static void SelectBuffers();

protected:
    static OpenGLBuffers tile;
};
