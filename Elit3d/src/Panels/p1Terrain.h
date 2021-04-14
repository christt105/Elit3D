#pragma once
#include "Panels/Base/Panel.h"

class r1Tileset3d;
class Viewport;

class p1Terrain :
    public Panel
{
public:
    p1Terrain(bool start_enabled = true, bool appear_mainmenubar = true, bool can_close = true);

    void Start() override;
    void Update() override;

    int selected = -1;
    r1Tileset3d* tileset = nullptr;

private:
    Viewport* viewport = nullptr;

    char bufname[30];
};
