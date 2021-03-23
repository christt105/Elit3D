#pragma once
#include "Resources/Base/Resource.h"

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

    std::vector<ObjectEditor*> meshes;
    r1Texture* texture = nullptr;
};

