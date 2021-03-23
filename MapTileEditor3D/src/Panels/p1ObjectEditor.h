#pragma once
#include "Panels/Base/Panel.h"
#include "Resources/r1Object.h"

class Viewport;
class r1Texture;

class p1ObjectEditor :
    public Panel
{
public:
    p1ObjectEditor(bool start_active, bool appear_mainmenubar, bool can_close);
    ~p1ObjectEditor();

    void Start() override;
    void Update() override;

private:
    void DrawObject();
    void ViewportWindow();
    void InfoWindow();

    void DrawUVs(float width, float height);

private:
    Viewport* viewport = nullptr;
    r1Object* object = nullptr; //resource?
    int selected = -1;
    int dragging = -1;
};
