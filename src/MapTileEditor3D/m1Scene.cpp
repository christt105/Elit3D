#include "m1Scene.h"

#include <GL/glew.h>
#include <SDL.h>

#include "Application.h"
#include "m1Input.h"
#include "m1Camera3D.h"

#include "m1Render3D.h"
#include "r1Shader.h"

#include "m1GUI.h"
#include "p1Scene.h"
#include "Viewport.h"

#include "m1Resources.h"
#include "r1Mesh.h"

// TEMP ====================================
#include "Object.h"
#include "c1Transform.h"
#include "c1Mesh.h"
#include "c1Material.h"

#include "r1Model.h"

#include "m1Window.h"

#include "ExternalTools/MathGeoLib/include/Geometry/Plane.h"

#include "m1Events.h"
//==========================================

#include "ExternalTools/MathGeoLib/include/Geometry/Ray.h"
#include "ExternalTools/MathGeoLib/include/Geometry/LineSegment.h"

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

	//((r1Model*)App->resources->Get(App->resources->Find("cubecat")))->CreateObject();

	return true;
}

UpdateStatus m1Scene::Update()
{
	PROFILE_FUNCTION();
	if (App->input->IsKeyDown(SDL_SCANCODE_ESCAPE))
		return UpdateStatus::UPDATE_STOP;

	App->gui->scene->viewport->Begin();

	if (draw_grid) {
		static auto shader1 = App->render->GetShader("grid");
		shader1->Use();
		glDrawArrays(GL_TRIANGLES, 0, 6);
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
		LOG(mouse_perc.ToString().c_str());

		if (App->input->IsMouseButtonDown(1)) {

			auto ray = App->camera->frustum.UnProject(mouse_perc.x, mouse_perc.y);
			
			/*xd0 = ray.a;
			xd1 = ray.b;*/

			xd0 = ray.pos;
			xd1 = ray.pos + ray.dir * 50.f;

			float t = 0.f;
			if (Plane::IntersectLinePlane(float3(0.f, 1.f, 0.f), 0.f, ray.pos, ray.dir, t) && t > 0.f) {
				float3 pos = ray.GetPoint(t);
				LOG("DING DING DING %s", pos.ToString().c_str());
				m1Events::Event* e = new m1Events::Event(m1Events::Event::Type::MOUSE_PICKING);
				e->info["collisionX"] = new fTypeVar(pos.x);
				e->info["collisionY"] = new fTypeVar(pos.y);
				e->info["collisionZ"] = new fTypeVar(pos.z);
				App->events->AddEvent(e);
			}
		}
	}

	glBegin(GL_LINES);
	glColor3f(1.0f, 0.f, 0.f);
	glVertex3f(xd0.x, xd0.y, xd0.z);
	glVertex3f(xd1.x, xd1.y, xd1.z);
	glEnd();

	App->gui->scene->viewport->End();
	
	return UpdateStatus::UPDATE_CONTINUE;
}

bool m1Scene::CleanUp()
{
	PROFILE_FUNCTION();

	return true;
}
