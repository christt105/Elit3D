#include "r1Mesh.h"

#include <GL/glew.h>

#include "Application.h"
#include "FileSystem.h"

#include "ExternalTools/Assimp/include/scene.h"

#include "ExternalTools/JSON/json.hpp"

#include "ExternalTools/mmgr/mmgr.h"

r1Mesh::r1Mesh(const uint64_t& id) : Resource(Resource::Type::Mesh, id)
{
}

r1Mesh::~r1Mesh()
{
}

void r1Mesh::Load()
{
	// fill buffers

	GenerateBuffers();
}

void r1Mesh::Unload()
{
	glDeleteVertexArrays(1, &VAO);

	glDeleteBuffers(1, &vertices.id);
	glDeleteBuffers(1, &indices.id);

	delete[] vertices.data;
	delete[] indices.data;
	if (texture.data != nullptr) {
		glDeleteBuffers(1, &texture.id);
		delete[] texture.data;
	}
}

void r1Mesh::GenerateFiles(const aiMesh* mesh)
{
	nlohmann::json file;

	file["nVertex"] = mesh->mNumVertices;
	for (unsigned int i = 0u; i < mesh->mNumVertices; ++i) {
		file["Vertex"].push_back(mesh->mVertices[i].x);
		file["Vertex"].push_back(mesh->mVertices[i].y);
		file["Vertex"].push_back(mesh->mVertices[i].z);
	}

	file["nFaces"] = mesh->mNumFaces;
	for (unsigned int i = 0u; i < mesh->mNumFaces; ++i) {
		for (unsigned int f = 0u; f < mesh->mFaces[i].mNumIndices; ++f) {
			file["Faces"].push_back(mesh->mFaces[i].mIndices[f]);
		}
	}

	App->file_system->SaveJSONFile(library_path.c_str(), file);
}

void r1Mesh::GenerateBuffers()
{
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
