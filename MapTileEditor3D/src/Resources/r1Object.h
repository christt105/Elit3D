#pragma once
#include "Resources/Base/Resource.h"
#include "Tools/OpenGL/Buffer.h"

#include <vector>

class r1Texture;
class ObjectEditor;

class r1Object :
    public Resource
{
public:
    r1Object(const uint64_t& uid);
    ~r1Object();

    void Load() override;
    void Unload() override;

    unsigned int VAO = 0u;

    Buffer<float> bvertices;
    Buffer<unsigned int> bindices;
    Buffer<float> btexture;

    std::vector<ObjectEditor*> meshes;
    r1Texture* texture = nullptr;
};

