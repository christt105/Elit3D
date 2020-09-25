#pragma once
#include "Panel.h"

#include <vector>

class Layer;

class p1Layers :
    public Panel
{
public:
    p1Layers(bool start_active = true, bool appear_mainmenubar = true, bool can_close = true);
    ~p1Layers();

    void Update() override;

    int GetSelected() const;

private:
    void DisplayLayers(std::vector<Layer*>* layers);
    void Buttons(std::vector<Layer*>*& layers);

private:
    int selected = -1;
};

