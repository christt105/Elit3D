#include "Panels/p1Layers.h"

#include "Core/Application.h"
#include "Modules/m1MapEditor.h"
#include "Modules/m1Events.h"

#include "Modules/m1GUI.h"
#include "Panels/p1Inspector.h"

#include "Modules/m1Objects.h"

#include "Tools/Map/MapLayer.h"
#include "Tools/Map/MapLayerObject.h"

#include "ExternalTools/ImGui/imgui_internal.h"

#include "Tools/System/Logger.h"

p1Layers::p1Layers(bool start_active, bool appear_mainmenubar, bool can_close)
	: Panel("Layers", start_active, appear_mainmenubar, can_close, ICON_FA_LAYER_GROUP)
{
}

p1Layers::~p1Layers() = default;

void p1Layers::Update()
{
	if (std::vector<MapLayer*>* layers = nullptr; App->map_editor->GetLayers(layers)) {
		Buttons(layers);
		ImGui::Separator(); //TODO: Child window
		DisplayLayers(layers);
	}
}

void p1Layers::DisplayLayers(std::vector<MapLayer*>* layers)
{
	ImGui::Columns(2, "##layers", false);
	ImGui::SetColumnWidth(0, 60.f);
	for (auto l = layers->rbegin(); l != layers->rend(); ++l) {
		ImGui::PushID(*l);
		bool l_selected = ((layers->size() - 1 - (l - layers->rbegin())) == selected);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.f, 0.f, 0.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.f, 0.f, 0.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.f, 0.f, 0.f));
		ImGui::AlignTextToFramePadding();
		if (!(*l)->visible)	ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 1);
		if (ImGui::Button(((*l)->visible) ? ICON_FA_EYE : ICON_FA_EYE_SLASH)) { (*l)->visible = !(*l)->visible; };
		ImGui::SameLine();
		bool locked = (*l)->locked;
		if (locked) {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.f, 0.f, 1.f));
		}
		if (ImGui::Button(((*l)->locked) ? ICON_FA_LOCK : ICON_FA_LOCK_OPEN)) { (*l)->locked = !(*l)->locked; };
		if (locked) {
			ImGui::PopStyleColor();
		}
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::NextColumn();
		ImGui::AlignTextToFramePadding();

		if ((*l)->locked) {
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}
		if (ImGui::Selectable((*l)->GetName(), l_selected)) {
			if (l_selected) {
				selected = -1;
				App->gui->inspector->SetSelected(nullptr, p1Inspector::SelectedType::NONE);
				if ((*l)->type == MapLayer::Type::OBJECT)
					App->objects->layer_root_selected = nullptr;
			}
			else {
				selected = layers->size() - (l - layers->rbegin()) - 1;
				App->gui->inspector->SetSelected(*l, p1Inspector::SelectedType::LAYER);
				if ((*l)->type == MapLayer::Type::OBJECT)
					App->objects->layer_root_selected = ((MapLayerObject*)(*l))->root;
				else App->objects->layer_root_selected = nullptr;
			}
		}
		ImVec2 s = ImGui::GetItemRectSize();
		ImGui::SameLine();
		ImGui::SetCursorPosX(s.x + 50.f);
		switch ((*l)->GetType())
		{
		case MapLayer::Type::TILE:
			ImGui::Text(ICON_FA_BORDER_ALL);
			break;
		case MapLayer::Type::OBJECT:
			ImGui::Text(ICON_FA_CUBE);
			break;
		case MapLayer::Type::TERRAIN:
			ImGui::Text(ICON_FA_CUBES); //ICON_FA_MOUNTAIN TODO: MACRO TERRAIN_ICON and other types
			break;
		default:
			LOGNW("Type %i not handled on switch", (*l)->GetType())
			break;
		}
		if ((*l)->locked) {
			ImGui::PopStyleVar();
		}
		ImGui::NextColumn();
		ImGui::Separator();
		ImGui::PopID();
	}
	ImGui::Columns();
}

void p1Layers::Buttons(std::vector<MapLayer*>*& layers)
{
	if (ImGui::Button(ICON_FA_PLUS)) {
		ImGui::OpenPopup("layer_type_create");
	}
	if (ImGui::BeginPopup("layer_type_create"))
	{
		if (ImGui::Button(ICON_FA_BORDER_ALL" Tile Layer")) {
			App->map_editor->AddLayer(MapLayer::Type::TILE);
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::Button(ICON_FA_CUBE" Object Layer")) {
			App->map_editor->AddLayer(MapLayer::Type::OBJECT);
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::Button(ICON_FA_CUBES" Terrain Layer")) {
			App->map_editor->AddLayer(MapLayer::Type::TERRAIN);
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
	ImGui::SameLine();
	bool disabled = (selected == -1 || selected > (int)layers->size());
	if (disabled) {
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}
	if (ImGui::Button(ICON_FA_TRASH_ALT)) {
		App->map_editor->EraseLayer(selected);
		selected = -1;
		App->gui->inspector->SetSelected(nullptr, p1Inspector::SelectedType::NONE);
		App->objects->layer_root_selected = nullptr;
	}
	if (disabled) {
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}

	ImGui::SameLine();

	if (disabled || selected == 0) {
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		disabled = true;
	}

	if (ImGui::Button(ICON_FA_ARROW_DOWN)) {
		auto tmp = (*layers)[selected];
		(*layers)[selected] = (*layers)[selected - 1];
		(*layers)[--selected] = tmp;
		App->events->AddEvent(new m1Events::Event(m1Events::Event::Type::REORDER_LAYERS));
	}

	if (disabled) {
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
		disabled = selected == -1 || selected > (int)layers->size();
	}

	ImGui::SameLine();

	if (disabled || selected == (int)layers->size() - 1) {
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		disabled = true;
	}
	if (ImGui::Button(ICON_FA_ARROW_UP)) {
		auto tmp = (*layers)[selected];
		(*layers)[selected] = (*layers)[selected + 1];
		(*layers)[++selected] = tmp;
		App->events->AddEvent(new m1Events::Event(m1Events::Event::Type::REORDER_LAYERS));
	}
	if (disabled) {
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}
}

int p1Layers::GetSelected() const
{
	return selected;
}

void p1Layers::SetSelected(int i)
{
	selected = i;
	
	if (std::vector<MapLayer*>* layers; App->map_editor->GetLayers(layers)) {
		MapLayer* layer = (*layers)[i];
		App->gui->inspector->SetSelected(layer, p1Inspector::SelectedType::LAYER);
		if (layer->type == MapLayer::Type::OBJECT) {
			App->objects->layer_root_selected = ((MapLayerObject*)layer)->root;
		}
	}
}
