#include "MapLayer.h"

#include <GL/glew.h>

#include "Application.h"
#include "m1Render3D.h"
#include "r1Shader.h"

#include "ExternalTools/MathGeoLib/include/Math/Quat.h"

#include "Profiler.h"

//#include "Logger.h"

OpenGLBuffers Layer::tile = OpenGLBuffers();

Layer::Layer()
{
	tile.InitData();
}

Layer::~Layer()
{
}

void Layer::Update()
{
	PROFILE_FUNCTION();

	float4x4 mat = float4x4::identity;
	mat = float4x4::FromTRS(float3(position.x * size, 0.f, position.y * size), Quat::identity, float3::one); // TODO: THIS UGLY
	static auto shader = App->render->GetShader("tilemap");
	shader->SetMat4("model", mat);
	glDrawElements(GL_TRIANGLES, tile.indices.size, GL_UNSIGNED_INT, (void*)0);
}

void Layer::SelectBuffers()
{
	glBindVertexArray(tile.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, tile.vertices.id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tile.indices.id);
}

OpenGLBuffers::OpenGLBuffers()
{
}

OpenGLBuffers::~OpenGLBuffers()
{
	glDeleteBuffers(1, &vertices.id);
	glDeleteBuffers(1, &indices.id);
	glDeleteBuffers(1, &texture.id);

	delete[] vertices.data;
	delete[] indices.data;
	delete[] texture.data;
}

void OpenGLBuffers::InitData()
{
	vertices.size = 4;
	vertices.data = new float[vertices.size * 3];

	float width = 8, height = 6;
	//width = height = 100;

	vertices.data[0] = 0.f;		vertices.data[1] = 0.f;		vertices.data[2] = 0.f;
	vertices.data[3] = width;	vertices.data[4] = 0.f;		vertices.data[5] = 0.f;
	vertices.data[6] = width;	vertices.data[7] = 0.f;		vertices.data[8] = height;
	vertices.data[9] = 0.f;		vertices.data[10] = 0.f;	vertices.data[11] = height;

	indices.size = 6;
	indices.data = new unsigned int[indices.size];

	indices.data[0] = 0u; indices.data[1] = 2u; indices.data[2] = 1u;
	indices.data[3] = 0u; indices.data[4] = 3u; indices.data[5] = 2u;

	texture.size = 4;
	texture.data = new float[texture.size * 2];
	memset(texture.data, 0.f, texture.size * 2 * sizeof(float));

	texture.data[0] = 0.f;		texture.data[1] = 0.f;
	texture.data[2] = 1.f;		texture.data[3] = 0.f;
	texture.data[4] = 1.f;		texture.data[5] = 1.f;
	texture.data[6] = 0.f;		texture.data[7] = 1.f;

	// VERTEX ARRAY OBJECT
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// VERTICES BUFFER
	glGenBuffers(1, &vertices.id);
	glBindBuffer(GL_ARRAY_BUFFER, vertices.id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size * 3, vertices.data, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	// TEXTURE COORDS BUFFER
	glGenBuffers(1, &texture.id);
	glBindBuffer(GL_ARRAY_BUFFER, texture.id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * texture.size, texture.data, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

	// INDICES BUFFER
	glGenBuffers(1, &indices.id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices.id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size, indices.data, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
