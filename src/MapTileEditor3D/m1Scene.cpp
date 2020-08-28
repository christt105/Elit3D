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
	GenerateGrid(); // TODO: Set grid in a shader

	panel_scene = App->gui->scene;

	//((r1Model*)App->resources->Get(App->resources->Find("cubecat")))->CreateObject();

	return true;
}

void m1Scene::GenerateGrid()
{
	int width = 100;
	grid_vertex_size = (width + 1) * 4 * 3; // width + 1(for the middle line) * 4(4 points by line) * 3(3 numbers per point)
	float* g = new float[grid_vertex_size];

	float y = 0.f;
	for (int i = 0; i <= width * 3 * 4; i += 12)
	{
		g[i] = i / 12;
		g[i + 1] = y;
		g[i + 2] = 0.f;

		g[i + 3] = i / 12;
		g[i + 4] = y;
		g[i + 5] = width;

		g[i + 6] = 0.f;
		g[i + 7] = y;
		g[i + 8] = i / 12;

		g[i + 9] = width;
		g[i + 10] = y;
		g[i + 11] = i / 12;
	}

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &grid);
	glBindBuffer(GL_ARRAY_BUFFER, grid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * grid_vertex_size, g, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	delete[] g;
}

UpdateStatus m1Scene::Update()
{
	PROFILE_FUNCTION();
	if (App->input->IsKeyDown(SDL_SCANCODE_ESCAPE))
		return UpdateStatus::UPDATE_STOP;

	App->gui->scene->viewport->Begin();

	static auto shader = App->render->GetShader("default");
	shader->Use();
	shader->SetMat4("model", float4x4::identity);

	if (draw_grid)
		DrawGrid();

	if(panel_scene->IsOnHover())
		if (App->input->IsMouseButtonDown(1)) {
			LOG("CLICK");
			/*auto l = App->camera->frustum.UnProjectLineSegment(0, 0);
			Ray r(l);*/

			
		}

	App->gui->scene->viewport->End();
	
	return UpdateStatus::UPDATE_CONTINUE;
}

void m1Scene::DrawGrid()
{
	glLineWidth(1.5f);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, grid);
	glDrawArrays(GL_LINES, 0, grid_vertex_size);
	glLineWidth(1.f);
}

bool m1Scene::CleanUp()
{
	PROFILE_FUNCTION();
	for (int i = 0; i < 10; ++i)
		delete map[i];

	return true;
}
