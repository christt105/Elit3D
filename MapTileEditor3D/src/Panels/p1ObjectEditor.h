#pragma once
#include "Panels/Base/Panel.h"

class Viewport;
class Object;
class r1Texture;

class p1ObjectEditor :
    public Panel
{
public:
    p1ObjectEditor(bool start_active, bool appear_mainmenubar, bool can_close);
    ~p1ObjectEditor();

    void Start() override;
    void Update() override;

    void ViewportWindow();

    void InfoWindow();

private:
    Viewport* viewport = nullptr;

    Object* object = nullptr;
    r1Texture* texture = nullptr;
};

