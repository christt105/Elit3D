#include "p1Layers.h"

#include "Application.h"
#include "m1MapEditor.h"

#include "MapLayer.h"

p1Layers::p1Layers(bool start_active, bool appear_mainmenubar, bool can_close)
	: Panel("Layers", start_active, appear_mainmenubar, can_close, ICON_FA_LAYER_GROUP)
{
}

p1Layers::~p1Layers()
{
}

void p1Layers::Update()
{
	std::vector<Layer*> layers;
	if (App->map_editor->GetLayers(layers)) {
		for (auto l = layers.begin(); l != layers.end(); ++l) {
			bool l_selected = ((l - layers.begin()) == selected);
			if (ImGui::Selectable((*l)->GetName(), l_selected)) {
				if (l_selected)
					selected = -1;
				else
					selected = l - layers.begin();
			}
		}
	}
}
