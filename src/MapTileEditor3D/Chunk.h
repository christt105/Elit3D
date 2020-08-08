#pragma once
#include "int2.h"
#include "Buffer.h"

class Chunk {
public:
    Chunk();
    ~Chunk();
    int2 position = int2(0, 0);

    int size = 8;

    void Update();

private:
    unsigned int VAO = 0u;

    Buffer<float> vertices;
    Buffer<unsigned int> indices;
    Buffer<float> texture;
};
