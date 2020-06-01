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
	Unload();
}

void r1Mesh::Load()
{
	auto file = App->file_system->OpenJSONFile(library_path.c_str());

	vertices.size = file["nVertex"];
	vertices.data = new float[vertices.size * 3];
	for (unsigned int i = 0; i < vertices.size * 3; ++i) {
		vertices.data[i] = file["Vertex"][i];
	}

	indices.size = file["nFaces"] * 3;
	indices.data = new unsigned int[indices.size];
	for (unsigned int i = 0; i < indices.size; ++i) {
		indices.data[i] = file["Faces"][i];
	}

	texture.size = vertices.size;
	texture.data = new float[texture.size * 2];
	for (unsigned int i = 0; i < texture.size * 2; ++i) {
		texture.data[i] = file["Tex"][i];
	}

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

	if (mesh->HasTextureCoords(0)) {
		for (unsigned int i = 0u; i < mesh->mNumVertices; ++i) {
			file["Tex"].push_back((float)mesh->mTextureCoords[0][i].x);
			file["Tex"].push_back((float)mesh->mTextureCoords[0][i].y);
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
