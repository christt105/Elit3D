#include "Chunk.h"

#include <GL/glew.h>

#include "Application.h"
#include "m1Render3D.h"
#include "r1Shader.h"

#include "ExternalTools/MathGeoLib/include/Math/Quat.h"

//#include "Logger.h"

Chunk::Chunk()
{
	vertices.size = size * size + 2 * size + 1;
	vertices.data = new float[vertices.size * 3];

	for (int i = 0; i < vertices.size * 3; i+=3) {
		vertices.data[i] = (float)((i/3)%(size+1));
		vertices.data[i+1] = 0.f;
		vertices.data[i+2] = (float)(int((i/3)/(size+1)));
	}

	/*for (int i = 0; i < vertices.size; i += 3) {
		LOG("V%i (%.2f, %.2f, %.2f)", i / 3, vertices.data[i], vertices.data[i + 1], vertices.data[i + 2]);
	}*/

	indices.size = size * size * 2 * 3;
	indices.data = new unsigned int[indices.size];

	for (int i = 0; i < indices.size; i += 6) {
		int index = (i / 6);
		/*

			    *
		      * *
		    * * *
		  * * * *
		* * * * *

		*/

		indices.data[i] = index + index / size;
		indices.data[i+1] = size + index + 2 + index / size;
		indices.data[i+2] = indices.data[i] + 1;

		/*

		* * * * *
		* * * *
		* * *
		* *
		*

		*/

		indices.data[i + 3] = indices.data[i];
		indices.data[i + 4] = indices.data[i + 1] - 1;
		indices.data[i + 5] = indices.data[i + 1];
	}

	/*for (int i = 0; i < indices.size; i += 6) {
		LOG("Q%i (%u, %u, %u / %u, %u, %u)", i / 6, indices.data[i], indices.data[i + 1], indices.data[i + 2],
			indices.data[i+3], indices.data[i + 4], indices.data[i + 5]);
	}*/

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
	if (texture.data != nullptr) {
		glGenBuffers(1, &texture.id);
		glBindBuffer(GL_ARRAY_BUFFER, texture.id);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * texture.size, texture.data, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(1);
	}

	// INDICES BUFFER
	glGenBuffers(1, &indices.id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices.id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size, indices.data, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Chunk::~Chunk()
{
	delete[] vertices.data;
	delete[] indices.data;
}

void Chunk::Update()
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, vertices.id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices.id);

	float4x4 mat = float4x4::FromTRS(float3(position.x, 0, position.y), Quat::identity, float3::one);
	
	App->render->bShader->SetMat4("model", mat);

	glDrawElements(GL_TRIANGLES, indices.size, GL_UNSIGNED_INT, (void*)0);
}
