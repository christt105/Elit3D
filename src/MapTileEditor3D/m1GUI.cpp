#include "m1GUI.h"

#include "ExternalTools/ImGui/imgui.h"
#include "ExternalTools/ImGui/imgui_impl_sdl.h"
#include "ExternalTools/ImGui/imgui_impl_opengl3.h"

#include "Application.h"
#include "m1Window.h"
#include "m1Render3D.h"

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
	return true;
}

bool m1GUI::Start()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	LOG("ImGui initialized with version %s", ImGui::GetVersion());

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->render->context);
	ImGui_ImplOpenGL3_Init("#version 330");

	return true;
}

UpdateStatus m1GUI::PreUpdate()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();
	return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus m1GUI::Update()
{
	if (ImGui::Begin("Application")) {
		ImGui::Text("DeltaTime: %f", App->GetDt());
		ImGui::End();
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
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	return true;
}
