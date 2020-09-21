#pragma once
#include "Panel.h"
class p1DebugResources :
    public Panel
{
public:
    p1DebugResources(bool start_enabled = true, bool appear_mainmenubar = true, bool can_close = true);
    ~p1DebugResources();

    void Update() override;
};

