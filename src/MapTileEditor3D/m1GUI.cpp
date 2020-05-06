#include "m1GUI.h"

#include "ExternalTools/ImGui/imgui.h"
#include "ExternalTools/ImGui/imgui_impl_sdl.h"
#include "ExternalTools/ImGui/imgui_impl_opengl3.h"

#include "Application.h"
#include "m1Window.h"
#include "m1Render3D.h"

#include "p1Configuration.h"
#include "p1About.h"

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

	panels.push_back(configuration);
	panels.push_back(about);

	return true;
}

bool m1GUI::Start()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	LOG("ImGui initialized with version %s", ImGui::GetVersion());

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiDockNodeFlags_PassthruCentralNode;
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.ConfigDockingWithShift = true;
	
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->render->context);
	ImGui_ImplOpenGL3_Init("#version 330");
	
	

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

	return UpdateStatus::UPDATE_CONTINUE;
}

void m1GUI::DockSpace()
{
	ImGuiViewport* viewport = ImGui::GetMainViewport();
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

UpdateStatus m1GUI::Update()
{
	if (demo)
		ImGui::ShowDemoWindow(&demo);

	for (auto i = panels.begin(); i != panels.end(); ++i) {
		ImGui::PushID(*i);
		if ((*i)->active) {
			if (ImGui::Begin((*i)->name.c_str(), &(*i)->active)) {
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

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	return true;
}
