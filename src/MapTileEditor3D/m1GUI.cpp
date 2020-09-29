#include "m1GUI.h"

#include "ExternalTools/ImGui/imgui.h"
#include "ExternalTools/ImGui/imgui_impl_sdl.h"
#include "ExternalTools/ImGui/imgui_impl_opengl3.h"

#include "Application.h"
#include "m1Window.h"
#include "m1Render3D.h"
#include "m1Events.h"
#include "m1MapEditor.h"
#include "r1Map.h"

#include "p1Configuration.h"
#include "p1About.h"
#include "p1Objects.h"
#include "p1Inspector.h"
#include "p1Console.h"
#include "p1Scene.h"
#include "p1Resources.h"
#include "p1Tileset.h"
#include "p1Layers.h"
#include "p1DebugResources.h"

#include "ExternalTools/ImGui/IconsFontAwesome5/IconsFontAwesome5.h"
#include "ExternalTools/ImGui/IconsFontAwesome5/IconsFontAwesome5Brands.h"

#include "Logger.h"

#include "Profiler.h"

#include "ExternalTools/mmgr/mmgr.h"

m1GUI::m1GUI(bool start_enabled) : Module("GUI", start_enabled)
{
}

m1GUI::~m1GUI()
{
}

bool m1GUI::Init(const nlohmann::json& node)
{
	PROFILE_FUNCTION();
	configuration = new p1Configuration();
	about = new p1About(false, false);
	objects = new p1Objects();
	inspector = new p1Inspector();
	console = new p1Console();
	scene = new p1Scene(true, false, false);
	resources = new p1Resources();
	tileset = new p1Tileset();
	layers = new p1Layers();
	dbg_resources = new p1DebugResources(false, false);

	panels.push_back(objects);
	panels.push_back(configuration);
	panels.push_back(inspector);
	panels.push_back(about);
	panels.push_back(console);
	panels.push_back(resources);
	panels.push_back(scene);
	panels.push_back(tileset);
	panels.push_back(layers);
	panels.push_back(dbg_resources);

	return true;
}

bool m1GUI::Start()
{
	PROFILE_FUNCTION();
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	LOGN("ImGui initialized with version %s", ImGui::GetVersion());

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.Fonts->AddFontDefault();

	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiDockNodeFlags_PassthruCentralNode;
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.ConfigDockingWithShift = true;

	// merge in icons from Font Awesome
	static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;
	io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FAS, 12.0f, &icons_config, icons_ranges);
	static const ImWchar icons_ranges2[] = { ICON_MIN_FAB, ICON_MAX_FAB , 0 };
	io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FAB, 16.0f, &icons_config, icons_ranges2);
	// use FONT_ICON_FILE_NAME_FAR if you want regular instead of solid
	
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->render->context);
	ImGui_ImplOpenGL3_Init("#version 330");
	
	for (auto i = panels.begin(); i != panels.end(); ++i) {
		(*i)->Start();
	}

	return true;
}

UpdateStatus m1GUI::PreUpdate()
{
	PROFILE_FUNCTION();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	if (ImGui::BeginMainMenuBar()) {
		MainMenuBar();

		ImGui::EndMainMenuBar();
	}

	DockSpace();

	return UpdateStatus::UPDATE_CONTINUE;
}

void m1GUI::MainMenuBar()
{
	PROFILE_FUNCTION();
	static bool create_map = false;
	if (ImGui::BeginMenu("File")) {
		if (ImGui::MenuItem("New Map")) {
			create_map = true;
		}
		
		if (ImGui::MenuItem("Save")) {
			App->events->AddEvent(new m1Events::Event(m1Events::Event::Type::SAVE_MAP));
		}

		ImGui::EndMenu();
	}

	if(create_map)
		if (ImGui::Begin("Create New Map", &create_map)) {
			static int size[2] = { 10 , 10 };
			ImGui::InputInt2("Size", size);

			if (ImGui::Button("Create")) {
				r1Map::CreateNewMap(size[0], size[1]);
				App->map_editor->ReLoadMap();
				create_map = false;
			}

			ImGui::End();
		}

	static bool resize_map = false;
	static int map_resize[2] = { 10 , 10 };
	if (ImGui::BeginMenu("Edit")) {
		if (ImGui::MenuItem("Resize Map")) {
			resize_map = true;
			int2 m = App->map_editor->GetMapSize();
			map_resize[0] = m.x; map_resize[1] = m.y;
		}

		ImGui::EndMenu();
	}

	if (resize_map)
		if (ImGui::Begin("Resize Map", &resize_map)) {
			ImGui::InputInt2("New Size", map_resize);

			if (ImGui::Button("Resize")) {
				m1Events::Event* e = new m1Events::Event(m1Events::Event::Type::RESIZE_MAP);
				e->info["width"] = new iTypeVar(map_resize[0]);
				e->info["height"] = new iTypeVar(map_resize[1]);
				App->events->AddEvent(e);
				resize_map = false;
			}

			ImGui::End();
		}

	if (ImGui::BeginMenu("Panels")) {
		for (auto i = panels.begin(); i != panels.end(); ++i) {
			if ((*i)->appear_in_mainmenubar) {
				ImGui::PushID(*i);
				if (ImGui::MenuItem((*i)->name.c_str(), "", (*i)->active)) {
					(*i)->active = !(*i)->active;
				}
				ImGui::PopID();
			}
		}
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Debugging")) {
		if (ImGui::MenuItem("Show Mouse Pick Line", "", App->debug.draw_mouse_pick_line)) {
			App->debug.draw_mouse_pick_line = !App->debug.draw_mouse_pick_line;
		}

		if (ImGui::MenuItem("Resources", "", dbg_resources->GetActive())) {
			dbg_resources->SetActive(!dbg_resources->GetActive());
		}

		if (ImGui::MenuItem("ImGui Demo Window", "", demo)) {
			demo = !demo;
		}

		if (ImGui::MenuItem("Save Map in image")) {
			App->map_editor->SaveImageMap();
		}

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Help")) {
		if (ImGui::MenuItem("About", "", about->active)) {
			about->active = !about->active;
		}
		ImGui::EndMenu();
	}
}

UpdateStatus m1GUI::Update()
{
	PROFILE_FUNCTION();
	if (demo)
		ImGui::ShowDemoWindow(&demo);

	for (auto i = panels.begin(); i != panels.end(); ++i) {
		ImGui::PushID(*i);
		if ((*i)->active) {
			if (ImGui::Begin(((*i)->icon + " " + (*i)->name).c_str(), ((*i)->can_be_closed) ? &(*i)->active : 0, (*i)->flags)) {
				(*i)->focused = ImGui::IsWindowFocused();
				(*i)->hover = ImGui::IsWindowHovered();

				(*i)->position.Set(ImGui::GetWindowPos());
				(*i)->size.Set(ImGui::GetWindowSize());

				(*i)->Update();
			}
			ImGui::End();
		}
		ImGui::PopID();
	}

	return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus m1GUI::PostUpdate()
{
	PROFILE_FUNCTION();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	return UpdateStatus::UPDATE_CONTINUE;
}

bool m1GUI::CleanUp()
{
	PROFILE_FUNCTION();
	for (auto i = panels.begin(); i != panels.end(); ++i) {
		delete* i;
	}
	console = nullptr;

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	return true;
}

void m1GUI::Save(nlohmann::json& node)
{
	/*for (auto i = panels.begin(); i != panels.end(); ++i) {
		(*i)->Save(node[(*i)->name]);
	}*/
}

void m1GUI::DockSpace()
{
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::SetNextWindowBgAlpha(0.0f);

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", NULL, window_flags);
	ImGui::PopStyleVar(3);

	ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
	ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	ImGui::End();
}
