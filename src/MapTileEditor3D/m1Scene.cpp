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

#include "m1Resources.h"
#include "r1Mesh.h"
#include "p1Tileset.h"

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

	auto r = (r1Mesh*)App->resources->EGet(m1Resources::EResourceType::TILE);
	r->Attach();

	return true;
}
#include "ExternalTools/MathGeoLib/include/Geometry/Plane.h"
#include "ExternalTools/MathGeoLib/include/Math/Quat.h"
UpdateStatus m1Scene::Update()
{
	PROFILE_FUNCTION();

	App->render->GetViewport("scene")->Begin();

	if (draw_grid) {
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

		auto ray = App->camera->frustum.UnProject(mouse_perc.x, mouse_perc.y);

		if (App->input->IsMouseButtonPressed(1)) {
			App->map_editor->MousePicking(ray);
			xd0 = ray.pos;
			xd1 = ray.pos + ray.dir * 50.f;
		}

		// Draw transparent tile on mouse
		if (App->gui->tileset->SelectTex()) {
			auto r = (r1Mesh*)App->resources->EGet(m1Resources::EResourceType::TILE);
			float t = 0.f;
			if (Plane::IntersectLinePlane(float3(0.f, 1.f, 0.f), 0, ray.pos, ray.dir, t)) {
				float3 position = ray.GetPoint(t);
				auto col = (int)floor(position.z - 0.5f);
				auto row = (int)floor(position.x);
				oglh::ActiveTexture(0);
				oglh::BindBuffers(r->VAO, r->vertices.id, r->indices.id);
				int2 tsize = App->gui->tileset->GetTilesetSize();

				r->texture.data[0] = 32.f / 256.f;
				r->texture.data[1] = 1952.f / 1984.f;

				r->texture.data[2] = 64.f / 256.f;
				r->texture.data[3] = 1952.f / 1984.f;

				r->texture.data[4] = 64.f / 256.f;
				r->texture.data[5] = 1984.f / 1984.f;

				r->texture.data[6] = 32.f / 256.f;
				r->texture.data[7] = 1984.f / 1984.f;

				oglh::GenArrayBuffer(r->texture.id, r->texture.size, sizeof(float), 2, r->texture.data, 1, 2);

				
				shader->SetMat4("model", float4x4::FromTRS(float3(row, 0.f, col), Quat::identity, float3::one));
				oglh::DrawElements(r->indices.size);
			}
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
