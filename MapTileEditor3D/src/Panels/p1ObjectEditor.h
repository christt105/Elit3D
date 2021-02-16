#pragma once
#include "Panels/Base/Panel.h"

class Viewport;
class r1Texture;
class ObjectEditor;

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

private:
    Viewport* viewport = nullptr;
    
    std::vector<ObjectEditor*> meshes;
    r1Texture* texture = nullptr;
};

