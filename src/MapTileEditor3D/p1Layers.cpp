#include "p1Layers.h"

#include "Application.h"
#include "m1MapEditor.h"

#include "m1GUI.h"
#include "p1Inspector.h"

#include "MapLayer.h"

#include "ExternalTools/ImGui/imgui_internal.h"

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
		if (ImGui::Button(ICON_FA_PLUS)) {
			App->map_editor->AddLayer();
		}
		ImGui::SameLine();
		if (selected == -1 || selected > layers.size()) {
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}
		if (ImGui::Button(ICON_FA_TRASH_ALT)) {
				App->map_editor->EraseLayer(selected);
		}
		if (selected == -1 || selected > layers.size()) {
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}

		ImGui::Separator();

		for (auto l = layers.rbegin(); l != layers.rend(); ++l) {
			bool l_selected = ((layers.size() - 1 - (l - layers.rbegin())) == selected);
			if (ImGui::Selectable((*l)->GetName(), l_selected)) {
				if (l_selected) {
					selected = -1;
					App->gui->inspector->SetSelected(nullptr, p1Inspector::SelectedType::NONE);
				}
				else {
					selected = layers.size() - (l - layers.rbegin()) - 1;
					App->gui->inspector->SetSelected(*l, p1Inspector::SelectedType::LAYER);
				}
			}
		}
	}
}

int p1Layers::GetSelected() const
{
	return selected;
}
