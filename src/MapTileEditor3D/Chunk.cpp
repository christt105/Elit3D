#include "Chunk.h"

#include <GL/glew.h>

#include "Application.h"
#include "m1Render3D.h"
#include "r1Shader.h"

#include "ExternalTools/MathGeoLib/include/Math/Quat.h"

//#include "Logger.h"

Chunk::Chunk()
{
	tiles = new int[size * size];
	memset(tiles, -1, size * size * sizeof(int));
}

Chunk::~Chunk()
{
	delete[] tiles;
}

void Chunk::Update()
{
	glBindVertexArray(tile.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, tile.vertices.id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tile.indices.id);
	
	float4x4 mat = float4x4::identity;
	for (int i = 0; i < size * size; ++i) {
		mat = float4x4::FromTRS(float3(i / size, 0.f, i % size), Quat::identity, float3::one);
		App->render->bShader->SetMat4("model", mat);
		glDrawElements(GL_TRIANGLES, tile.indices.size, GL_UNSIGNED_INT, (void*)0);
	}
}

Tile::Tile()
{
	vertices.size = 4;
	vertices.data = new float[vertices.size * 3];

	vertices.data[0] = 0.f; vertices.data[1] = 0.f; vertices.data[2] = 0.f;
	vertices.data[3] = 1.f; vertices.data[4] = 0.f; vertices.data[5] = 0.f;
	vertices.data[6] = 1.f; vertices.data[7] = 0.f; vertices.data[8] = 1.f;
	vertices.data[9] = 0.f; vertices.data[10] = 0.f; vertices.data[11] = 1.f;

	indices.size = 6;
	indices.data = new unsigned int[indices.size];

	indices.data[0] = 0u; indices.data[1] = 2u; indices.data[2] = 1u;
	indices.data[3] = 0u; indices.data[4] = 3u; indices.data[5] = 2u;

	texture.size = 4;
	texture.data = new float[texture.size * 2];
	memset(texture.data, 0.f, texture.size * 2 * sizeof(float));

	texture.data[0] = 0.f;		texture.data[1] = 1.f - 1.f / 625.f;
	texture.data[2] = 0.125f;	texture.data[3] = 1.f - 1.f / 625.f;
	texture.data[4] = 0.125f;	texture.data[5] = 1.f;
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

Tile::~Tile()
{
	glDeleteBuffers(1, &vertices.id);
	glDeleteBuffers(1, &indices.id);
	glDeleteBuffers(1, &texture.id);

	delete[] vertices.data;
	delete[] indices.data;
	delete[] texture.data;
}
