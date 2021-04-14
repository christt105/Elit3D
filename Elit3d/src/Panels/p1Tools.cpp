#include "Panels/p1Tools.h"
#include "Modules/m1GUI.h"

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
	//tools[Tools::RECTANGLE] = ICON_FA_SQUARE;
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

	static const char* charShapes[] = { ICON_FA_SQUARE, ICON_FA_CIRCLE };

	switch (selectedTool)
	{
	case p1Tools::Tools::BRUSH:
	case p1Tools::Tools::ERASER:
	case p1Tools::Tools::RECTANGLE:
		ImGui::SameLine();
		ImGui::Text("|");
		ImGui::SameLine();
		if (selectedTool != p1Tools::Tools::RECTANGLE) {
			ImGui::SetNextItemWidth(130.f);
			if (ImGui::InputInt("Size", &brushSize, 1, 5)) {
				if (brushSize < 1)
					brushSize = 1;
			}
			ImGui::SameLine();
		}
		ImGui::SetNextItemWidth(50.f);
		if (ImGui::BeginCombo("Shape", charShapes[(int)shape])) {
			for (int i = 0; i < 2; ++i)
				if (ImGui::Selectable(charShapes[i], i == (int)shape)) {
					shape = (Shape)i;
				}
			ImGui::EndCombo();
		}
		break;
	default:
		break;
	}
}

p1Tools::Tools p1Tools::GetSelectedTool() const
{
	return selectedTool;
}

int p1Tools::GetToolSize() const
{
	switch (selectedTool)
	{
	case p1Tools::Tools::BRUSH:
	case p1Tools::Tools::ERASER:
		return brushSize;
	default:
		return 1;
	}
}

p1Tools::Shape p1Tools::GetToolShape() const
{
	switch (selectedTool)
	{
	case p1Tools::Tools::BRUSH:
	case p1Tools::Tools::ERASER:
		return shape;
	default:
		return Shape::RECTANGLE;
	}
}
