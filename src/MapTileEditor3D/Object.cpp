#include "Object.h"
#include <GL/glew.h>

#include "Shader.h"

Object::Object()
{
}

Object::~Object()
{
	delete[] vertices.data;
	delete[] indices.data;
}

void Object::GenerateBuffers()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &vertices.id);
	glBindBuffer(GL_ARRAY_BUFFER, vertices.id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size * 3, vertices.data, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);
	
	glGenBuffers(1, &indices.id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices.id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size, indices.data, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Object::Draw()
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, vertices.id);
	glVertexPointer(3, GL_FLOAT, 0, (void*)0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices.id);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	material.shader->SetVec3("color", float3::one);
	glDrawElements(GL_TRIANGLES, indices.size, GL_UNSIGNED_INT, (void*)0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPolygonOffset(-1.f, 1.f);
	glLineWidth(1.5f);
	material.shader->SetVec3("color", float3::zero);
	glDrawElements(GL_TRIANGLES, indices.size, GL_UNSIGNED_INT, (void*)0);
	glLineWidth(1.f);
}
