#include "m1Scene.h"

#include <GL/glew.h>
#include <SDL.h>

#include "Application.h"
#include "m1Input.h"

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
	int width = 10;
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

	return true;
}

UpdateStatus m1Scene::Update()
{
	if (App->input->IsKeyDown(SDL_SCANCODE_ESCAPE))
		return UpdateStatus::UPDATE_STOP;

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, grid);
	glDrawArrays(GL_LINES, 0, grid_vertex_size);

	return UpdateStatus::UPDATE_CONTINUE;
}

bool m1Scene::CleanUp()
{
	return true;
}
