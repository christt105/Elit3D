#include "p1Tools.h"
#include "m1GUI.h"

p1Tools::p1Tools(bool start_active, bool appear_mainmenubar, bool can_close) 
	: Panel("Tools", start_active, appear_mainmenubar, can_close, ICON_FA_TOOLBOX)
{
}

p1Tools::~p1Tools()
{
}

void p1Tools::Start()
{
	tools[Tools::BRUSH] = ICON_FA_PAINT_BRUSH;
	tools[Tools::ERASER] = ICON_FA_ERASER;
	tools[Tools::BUCKET] = ICON_FA_FILL_DRIP;
	tools[Tools::EYEDROPPER] = ICON_FA_EYE_DROPPER;
	tools[Tools::RECTANGLE] = ICON_FA_SQUARE;
}

void p1Tools::Update()
{
	for (auto i = tools.begin(); i != tools.end(); ++i) {
		bool selected = false;
		if (selectedTool == (*i).first) {
			selected = true;
			ImGui::PushStyleColor(ImGuiCol_Button, ORANGE);
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, ORANGE);
		}
		if (ImGui::Button((*i).second.c_str())) {
			selectedTool = (*i).first;
		}
		if (selected) {
			ImGui::PopStyleColor(2);
		}
		if (i != (--tools.end()))
			ImGui::SameLine();
	}
}

p1Tools::Tools p1Tools::GetSelectedTool() const
{
	return selectedTool;
}
