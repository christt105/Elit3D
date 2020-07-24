#pragma once

#include "Module.h"
#include "int2.h"

class Object;
class p1Scene;

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
    int* map = nullptr;
    int2 size = { 10,10 };
    Object** tiles = nullptr;

    p1Scene* panel_scene = nullptr;
};

