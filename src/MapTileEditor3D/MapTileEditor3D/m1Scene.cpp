#include "m1Scene.h"

#include <GL/glew.h>

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
	vertices = new float[3 * 3];
	vertices[0] = -0.5;	vertices[1] = -0.5;	vertices[2] = 0.0;
	vertices[3] =  0.5;	vertices[4] = -0.5;	vertices[5] = 0.0;
	vertices[6] =  0.0;	vertices[7] =  0.5;	vertices[8] = 0.0;

	indices = new unsigned int[3];
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9, vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, NULL);

	glBindVertexArray(NULL);

	return true;
}

UpdateStatus m1Scene::Update()
{

	if (App->input->IsKeyDown(SDL_SCANCODE_A))
		LOG("A PRESSED");

	glBindVertexArray(VAO);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

	return UpdateStatus::UPDATE_CONTINUE;
}

bool m1Scene::CleanUp()
{
	delete[] vertices;
	delete[] indices;

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	return true;
}
