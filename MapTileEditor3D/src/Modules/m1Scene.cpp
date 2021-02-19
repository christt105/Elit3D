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

	return true;
}

UpdateStatus m1Scene::Update()
{
	PROFILE_FUNCTION();

	{
		static Viewport const * v = App->render->GetViewport("scene");
		v->Begin();

		if (draw_grid) {
			static auto shader1 = App->render->GetShader("grid");
			shader1->Use();
			oglh::DepthEnable(true);
			oglh::DrawArrays(6);
			oglh::DepthEnable(false);
		}

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

			auto ray = v->GetCamera()->frustum.UnProject(mouse_perc);
			App->map_editor->Mouse(ray);
		}

		v->End();
	}

	{
		static Viewport const * v = App->render->GetViewport("object editor");
		v->Begin();

		static auto shader1 = App->render->GetShader("grid");
		shader1->Use();
		oglh::DepthEnable(true);
		oglh::DrawArrays(6);
		oglh::DepthEnable(false);

		v->End();
	}
	
	return UpdateStatus::UPDATE_CONTINUE;
}

bool m1Scene::CleanUp()
{
	PROFILE_FUNCTION();

	return true;
}
