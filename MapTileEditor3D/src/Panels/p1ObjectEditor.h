#pragma once
#include "Panels/Base/Panel.h"

class Viewport;

class p1ObjectEditor :
    public Panel
{
public:
    p1ObjectEditor(bool start_active, bool appear_mainmenubar, bool can_close);
    ~p1ObjectEditor() = default;

    void Start() override;
    void Update() override;

private:
    Viewport* viewport = nullptr;
};

