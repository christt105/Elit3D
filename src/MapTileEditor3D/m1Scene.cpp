#include "m1Scene.h"

#include <GL/glew.h>
#include <SDL.h>

#include "Application.h"
#include "m1Input.h"
#include "m1Camera3D.h"

#include "m1GUI.h"
#include "p1Scene.h"

#include "m1Resources.h"
#include "r1Mesh.h"

// TEMP ====================================
#include "Object.h"
#include "c1Transform.h"
#include "c1Mesh.h"
#include "c1Material.h"
//==========================================

#include "ExternalTools/MathGeoLib/include/Geometry/Ray.h"
#include "ExternalTools/MathGeoLib/include/Geometry/LineSegment.h"

#include "Logger.h"

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
	GenerateGrid();

	panel_scene = App->gui->scene;

	//((r1Model*)App->resources->Get(App->resources->Find("cubecat")))->CreateObject();

	for (int i = 0; i < 10; ++i) {
		map[i] = new Object();
		map[i]->transform->SetPosition(float3(i, 0.f, 0.f));
		map[i]->CreateComponent<c1Mesh>()->SetEMesh(m1Resources::EResourceType::TILE);
		//map[i]->GetComponent<c1Material>()
	}

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
	if (App->input->IsKeyDown(SDL_SCANCODE_ESCAPE))
		return UpdateStatus::UPDATE_STOP;

	DrawGrid();

	if(panel_scene->IsOnHover())
		if (App->input->IsMouseButtonDown(1)) {
			LOG("CLICK");
			/*auto l = App->camera->frustum.UnProjectLineSegment(0, 0);
			Ray r(l);*/

			map[0]->GetComponent<c1Material>()->SetTexture("test2");
			auto mesh = map[0]->GetComponent<c1Mesh>();
			float* t = new float[2 * 4];
			t[0] = 0.f; t[1] = 1.f - 1.f / 625.f;
			t[2] = 0.125f; t[3] = 1.f - 1.f / 625.f;
			t[4] = 0.125f; t[5] = 1.f;
			t[6] = 0.f; t[7] = 1.f;
			glBindBuffer(GL_ARRAY_BUFFER, mesh->emesh->texture.id);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * mesh->emesh->texture.size, t, GL_STATIC_DRAW);
			delete[] t;
		}

	for (int i = 0; i < 10; ++i) {
		map[i]->GetComponent<c1Mesh>()->Update();
	}
	
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
	for (int i = 0; i < 10; ++i)
		delete map[i];

	return true;
}
