#pragma once

#include "Module.h"
#include "int2.h"
#include "Buffer.h"

class Object;
class p1Scene;
class Chunk;

class m1MapEditor :
    public Module
{
public:
    m1MapEditor();
    ~m1MapEditor();

    bool Start() override;
    UpdateStatus Update() override;
    bool CleanUp() override;

private:
    Chunk* chunks = nullptr;
    
    p1Scene* panel_scene = nullptr;
};

