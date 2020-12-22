#pragma once
#include "Panel.h"

class p1Tools :
    public Panel
{
public:
    enum class Tools {
        NONE = -1,

        BRUSH,
        ERASER,
        BUCKET,
        EYEDROPPER,
        RECTANGLE,

        MAX
    };
    p1Tools(bool start_active, bool appear_mainmenubar, bool can_close);
    ~p1Tools();

    void Start() override;
    void Update() override;
    Tools GetSelectedTool() const;

    Tools selectedTool = Tools::NONE;

private:
    std::map<Tools, std::string> tools;
};

