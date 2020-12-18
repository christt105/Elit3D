#include "p1Scene.h"

#include "Application.h"
#include "m1Window.h"
#include "m1Camera3D.h"
#include "m1Render3D.h"
#include "m1MapEditor.h"
#include "m1Resources.h"

#include "r1Map.h"

#include "Viewport.h"

#include "Logger.h"

#include "ExternalTools/ImGui/imgui_internal.h"




//temp 
#include "ExternalTools/MathGeoLib/include/Math/MathFunc.h"

p1Scene::p1Scene(bool start_enabled, bool appear_mainmenubar, bool can_close)
	: Panel("Scene", start_enabled, appear_mainmenubar, can_close, ICON_FA_CUBES)
{
	flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse 
		| ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse 
		| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoDecoration;
}

void p1Scene::Start()
{
	viewport = App->render->CreateViewport("scene");
}

p1Scene::~p1Scene()
{
}

void p1Scene::Update()
{
	MenuBar();

	if (App->map_editor->ValidMap()) {

		ImGui::PushClipRect(ImGui::GetWindowPos(), ImGui::GetWindowPos() + ImGui::GetWindowSize(), false);
		ImGui::SetCursorScreenPos(ImGui::GetWindowPos() + ImVec2(0, ImGui::GetCurrentWindow()->TitleBarHeight() + ImGui::GetCurrentWindow()->MenuBarHeight()));

		ImVec2 window_size = ImGui::GetContentRegionAvail() + ImVec2(16, 16);
		viewport->UpdateSize((int)window_size.x, (int)window_size.y); // TODO: Extract to a viewport function
		//App->camera->frustum.verticalFov = DegToRad(60.f);
		//App->camera->frustum.horizontalFov = 2.f * atanf(tanf(App->camera->frustum.verticalFov * 0.5f) * (window_size.x / window_size.y));
		viewport->Update();

		viewport->Blit();

		ImGui::Image((ImTextureID)viewport->GetTexture(), window_size, ImVec2(0, 0), ImVec2(1, -1));

		ImGui::PopClipRect();
	}
	else {
		ShowCreateMap();
	}
}

void p1Scene::ShowCreateMap()
{
	static bool popup_create = false;
	static bool popup_load = false;
	ImGui::SetCursorScreenPos(ImGui::GetWindowPos() + ImGui::GetWindowSize() * 0.5f);
	if (ImGui::Button("Create Map", ImVec2(100.f, 20.f))) {
		ImGui::OpenPopup("Create Map");
		popup_create = true;
	}
	ImGui::SetCursorScreenPos(ImGui::GetWindowPos() + ImGui::GetWindowSize() * 0.5f + ImVec2(45.f, 25.f));
	ImGui::Text("or");
	ImGui::SetCursorScreenPos(ImGui::GetWindowPos() + ImGui::GetWindowSize() * 0.5f + ImVec2(0.f, 40.f));
	if (ImGui::Button("Load Map", ImVec2(100.f, 20.f))) {
		ImGui::OpenPopup("Load Map");
		popup_load = true;
	}
	ImGui::SetNextWindowSize(ImVec2(350.f, 250.f), ImGuiCond_Always);
	ImGui::SetNextWindowPos(ImVec2((float)App->window->GetWidth() * 0.5f - 350.f * 0.5f, (float)App->window->GetHeight() * 0.5f - 250.f * 0.5f));
	if (ImGui::BeginPopupModal("Create Map", &popup_create, ImGuiWindowFlags_::ImGuiWindowFlags_NoMove)) {
		PopUpCreateMap();
		ImGui::EndPopup();
	}

	ImGui::SetNextWindowSize(ImVec2(350.f, 250.f), ImGuiCond_Always);
	ImGui::SetNextWindowPos(ImVec2((float)App->window->GetWidth() * 0.5f - 350.f * 0.5f, (float)App->window->GetHeight() * 0.5f - 250.f * 0.5f));
	if (ImGui::BeginPopupModal("Load Map", &popup_load)) {
		PopUpLoadMap();
		ImGui::EndPopup();
	}
}

void p1Scene::PopUpCreateMap()
{
	static char name[30];
	ImGui::InputText("Name:", name, 30);

	ImGui::Text("Size:");
	static int size[2] = { 10, 10 };
	ImGui::InputInt("Width", &size[0]);
	ImGui::InputInt("Height", &size[1]);

	ImGui::NewLine();

	if (ImGui::Button("Save")) {
		if (!std::string(name).empty()) {
			std::string path = ("./Assets/Maps/" + std::string(name) + ".scene");
			if (App->resources->FindByPath(path.c_str()) != 0ULL) {
				int repeat = 0;
				while (App->resources->FindByPath(path.c_str()) != 0ULL) {
					path = "./Assets/Maps/" + std::string(name) + "(" + std::to_string(repeat++) + ")" + ".scene";
				}
			}
			App->resources->PauseFileWatcher(true);
			r1Map::CreateNewMap(size[0], size[1], path.c_str());
			auto m = App->resources->CreateResource<r1Map>(path.c_str());
			App->resources->GenerateMeta(path.c_str());
			App->map_editor->LoadMap(m->GetUID());
			App->resources->PauseFileWatcher(false);
			ImGui::CloseCurrentPopup();
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel")) {
		ImGui::CloseCurrentPopup();
	}
}

void p1Scene::PopUpLoadMap()
{
	auto maps = App->resources->GetVectorOf(Resource::Type::Map);

	for (auto i = maps.begin(); i != maps.end(); ++i) {
		if (ImGui::Button((*i)->name.c_str())) {
			App->map_editor->LoadMap((*i)->GetUID());
		}
	}
}

void p1Scene::MenuBar()
{
	if (ImGui::BeginMenuBar()) {
		/*if (App->camera->frustum.type == FrustumType::PerspectiveFrustum) {
			if (ImGui::Button("2D")) {
				App->camera->frustum.front = -float3::unitY;
				App->camera->frustum.up = float3::unitX;
				App->camera->frustum.type = FrustumType::OrthographicFrustum;
				App->camera->frustum.orthographicHeight = 10;
				App->camera->frustum.orthographicWidth = 20 / App->camera->frustum.AspectRatio();
			}
		}
		else {
			if (ImGui::Button("3D")) {
				App->camera->frustum.type = FrustumType::PerspectiveFrustum;
				App->camera->SetFov();
			}
		}*/

		ImGui::EndMenuBar();
	}
}
