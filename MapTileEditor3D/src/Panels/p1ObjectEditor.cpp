#include "p1ObjectEditor.h"
#include "Core/Application.h"
#include "Modules/m1Render3D.h"
#include "Tools/OpenGL/Viewport.h"
#include "ExternalTools/ImGui/imgui_internal.h"

p1ObjectEditor::p1ObjectEditor(bool start_active, bool appear_mainmenubar, bool can_close)
	: Panel("Object Editor", start_active, appear_mainmenubar, can_close, ICON_FA_DRAW_POLYGON /*other: https://fontawesome.com/icons/pencil-ruler?style=solid | https://fontawesome.com/icons/tree?style=solid | https://fontawesome.com/icons/hammer?style=solid*/)
{
}

void p1ObjectEditor::Start()
{
	viewport = App->render->CreateViewport("object editor");
}

void p1ObjectEditor::Update()
{
	{
		ImGui::BeginChild("childObjectInfo", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.3f, 0.f));
		ImGui::Text("childObjectInfo");

		ImGui::EndChild();
	}
	ImGui::SameLine();
	{
		ImGui::BeginChild("childObjectViewport", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.7f, 0.f));
		
		ImGui::PushClipRect(ImGui::GetWindowPos(), ImGui::GetWindowPos() + ImGui::GetWindowSize(), false);
		ImGui::SetCursorScreenPos(ImGui::GetWindowPos() + ImVec2(0, ImGui::GetCurrentWindow()->TitleBarHeight() + ImGui::GetCurrentWindow()->MenuBarHeight()));

		ImVec2 window_size = ImGui::GetContentRegionAvail() + ImVec2(16, 16);
		viewport->UpdateSize((int)window_size.x, (int)window_size.y); // TODO: Extract to a viewport function
		
		viewport->Update();

		viewport->Blit();

		ImGui::Image((ImTextureID)viewport->GetTexture(), window_size, ImVec2(0, 0), ImVec2(1, -1));

		ImGui::PopClipRect();

		ImGui::EndChild();
	}
}
