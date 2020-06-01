#include "m1GUI.h"

#include "ExternalTools/ImGui/imgui.h"
#include "ExternalTools/ImGui/imgui_impl_sdl.h"
#include "ExternalTools/ImGui/imgui_impl_opengl3.h"

#include "Application.h"
#include "m1Window.h"
#include "m1Render3D.h"

#include "p1Configuration.h"
#include "p1About.h"
#include "p1Objects.h"
#include "p1Inspector.h"
#include "p1Console.h"
#include "p1Scene.h"
#include "p1Resources.h"
#include "p1Tileset.h"

#include "ExternalTools/ImGui/IconsFontAwesome5/IconsFontAwesome5.h"

#include "Logger.h"

#include "ExternalTools/mmgr/mmgr.h"

m1GUI::m1GUI(bool start_enabled) : Module("GUI", start_enabled)
{
}

m1GUI::~m1GUI()
{
}

bool m1GUI::Init(const nlohmann::json& node)
{
	configuration = new p1Configuration();
	about = new p1About(false);
	objects = new p1Objects();
	inspector = new p1Inspector();
	console = new p1Console();
	scene = new p1Scene();
	resources = new p1Resources();
	tileset = new p1Tileset();

	panels.push_back(configuration);
	panels.push_back(objects);
	panels.push_back(inspector);
	panels.push_back(about);
	panels.push_back(console);
	panels.push_back(resources);
	panels.push_back(scene);
	panels.push_back(tileset);

	return true;
}

bool m1GUI::Start()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	LOG("ImGui initialized with version %s", ImGui::GetVersion());

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
	// use FONT_ICON_FILE_NAME_FAR if you want regular instead of solid
	
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->render->context);
	ImGui_ImplOpenGL3_Init("#version 330");
	
	scene->InitFrameBuffer();

	return true;
}

UpdateStatus m1GUI::PreUpdate()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Panels")) {
			for (auto i = panels.begin(); i != panels.end(); ++i) {
				if (*i != about) {
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
			if (ImGui::MenuItem("ImGui Demo Window", "", demo)) {
				demo = !demo;
			}
			ImGui::EndMenu();
		}
		
		if (ImGui::BeginMenu("Help")) {
			if (ImGui::MenuItem("About", "", about->active)) {
				about->active = !about->active;
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	DockSpace();
	
	scene->SelectFrameBuffer();

	return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus m1GUI::Update()
{
	if (demo)
		ImGui::ShowDemoWindow(&demo);

	scene->DeselectFrameBuffer();

	for (auto i = panels.begin(); i != panels.end(); ++i) {
		ImGui::PushID(*i);
		if ((*i)->active) {
			if (ImGui::Begin(((*i)->icon + " " + (*i)->name).c_str(), &(*i)->active, (*i)->flags)) {
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
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	return UpdateStatus::UPDATE_CONTINUE;
}

bool m1GUI::CleanUp()
{
	for (auto i = panels.begin(); i != panels.end(); ++i) {
		delete* i;
	}
	console = nullptr;

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	return true;
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
