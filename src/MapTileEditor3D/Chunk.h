#pragma once
#include "int2.h"
#include "Buffer.h"

class r1Mesh;

class Tile {
public:
    Tile();
    ~Tile();

    unsigned int VAO = 0u;

    Buffer<float> vertices;
    Buffer<unsigned int> indices;
    Buffer<float> texture;
};

class Chunk {
public:
    Chunk();
    ~Chunk();
    int2 position = int2(0, 0);

    int size = 8;

    void Update();

private:
    Tile tile;
    int* tiles = nullptr;
};
