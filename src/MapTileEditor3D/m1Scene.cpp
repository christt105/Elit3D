#include "m1Scene.h"

#include "Application.h"
#include "m1Input.h"
#include "m1Camera3D.h"

#include "OpenGLHelper.h"

#include "m1Render3D.h"
#include "r1Shader.h"

#include "m1GUI.h"
#include "p1Scene.h"
#include "Viewport.h"

#include "m1MapEditor.h"

#include "Logger.h"

#include "Profiler.h"

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

	return true;
}

UpdateStatus m1Scene::Update()
{
	PROFILE_FUNCTION();

	App->render->GetViewport("scene")->Begin();

	if (draw_grid) {
		oglh::UnBindBuffers();
		static auto shader1 = App->render->GetShader("grid");
		shader1->Use();
		oglh::DrawArrays(6);
	}

	static auto shader = App->render->GetShader("default");
	shader->Use();
	shader->SetMat4("model", float4x4::identity);

	static float3 xd0 = float3::zero;
	static float3 xd1 = float3::one;
	if (panel_scene->IsOnHover()) {
		App->input->GetMousePosition(&last_mouse_click.x, &last_mouse_click.y);
		int2 mouse_pos = last_mouse_click - panel_scene->GetPosition() - int2(0, 16 + 15);
		mouse_pos.y = panel_scene->GetSize().y - mouse_pos.y;
		float2 mouse_perc(2 * ((float)mouse_pos.x) / ((float)panel_scene->GetSize().x) - 1, 2 * ((float)mouse_pos.y) / ((float)panel_scene->GetSize().y) - 1);

		if (App->input->IsMouseButtonPressed(1)) {

			auto ray = App->camera->frustum.UnProject(mouse_perc.x, mouse_perc.y);

			App->map_editor->MousePicking(ray);
			xd0 = ray.pos;
			xd1 = ray.pos + ray.dir * 50.f;
		}
	}

	if (App->debug.draw_mouse_pick_line) {
		oglh::OldDrawLines(xd0, xd1);
	}

	App->render->GetViewport("scene")->End();
	
	return UpdateStatus::UPDATE_CONTINUE;
}

bool m1Scene::CleanUp()
{
	PROFILE_FUNCTION();

	return true;
}
