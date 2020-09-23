#pragma once
#include "Panel.h"
class p1Layers :
    public Panel
{
public:
    p1Layers(bool start_active = true, bool appear_mainmenubar = true, bool can_close = true);
    ~p1Layers();

    void Update() override;

private:
    int selected = -1;
};

