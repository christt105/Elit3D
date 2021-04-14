#pragma once
#include "Panels/Base/Panel.h"

#include <vector>

class MapLayer;

class p1Layers :
    public Panel
{
public:
    p1Layers(bool start_active = true, bool appear_mainmenubar = true, bool can_close = true);
    ~p1Layers();

    void Update() override;

    int GetSelected() const;
    void SetSelected(int i);

private:
    void DisplayLayers(std::vector<MapLayer*>* layers);
    void Buttons(std::vector<MapLayer*>*& layers);

private:
    int selected = -1;
};

