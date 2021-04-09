#include "Modules/m1Scene.h"

#include "Core/Application.h"
#include "Modules/m1Input.h"
#include "Modules/m1Camera3D.h"

#include "Tools/OpenGL/OpenGLHelper.h"

#include "Modules/m1Render3D.h"
#include "Resources/r1Shader.h"

#include "Modules/m1GUI.h"
#include "Panels/p1Scene.h"
#include "Tools/OpenGL/Viewport.h"

#include "Modules/m1MapEditor.h"

#include "Tools/System/Logger.h"

#include "Tools/System/Profiler.h"

#include "ExternalTools/mmgr/mmgr.h"

m1Scene::m1Scene(bool start_enabled) : Module("Scene", start_enabled)
{
}

m1Scene::~m1Scene()
{
}

bool m1Scene::Init(const nlohmann::json& node)
{
	return true;
}

bool m1Scene::Start()
{
	PROFILE_FUNCTION();

	panel_scene = App->gui->scene;
	sceneViewport = App->render->GetViewport("scene");

	return true;
}

UpdateStatus m1Scene::Update()
{
	PROFILE_FUNCTION();

	if(sceneViewport == nullptr)
		sceneViewport = App->render->GetViewport("scene");

	sceneViewport->Begin();

	static auto shader = App->render->GetShader("default");
	shader->Use();
	shader->SetMat4("model", float4x4::identity);

	static float3 xd0 = float3::zero;
	static float3 xd1 = float3::one;
	if (panel_scene->IsOnHover()) {
		int2 mouse_panel = panel_scene->GetMousePosition();
		int2 size_panel = panel_scene->GetSize();
		mouse_panel.y = size_panel.y - mouse_panel.y;
		float2 mouse_perc(2 * ((float)mouse_panel.x) / ((float)size_panel.x) - 1, 2 * ((float)mouse_panel.y) / ((float)size_panel.y) - 1);

		auto ray = App->camera->scene->GetFrustum().UnProject(mouse_perc.x, mouse_perc.y);
		App->map_editor->Mouse(ray);
		//shader->Use();

		/*if (App->input->IsMouseButtonPressed(1)) {
			xd0 = ray.pos;
			xd1 = ray.pos + ray.dir * 250.f;
		}*/
	}

	if (App->debug.draw_mouse_pick_line) {
		oglh::OldDrawLines(xd0, xd1);
	}

	sceneViewport->End();

	return UpdateStatus::UPDATE_CONTINUE;
}

bool m1Scene::CleanUp()
{
	PROFILE_FUNCTION();

	return true;
}
