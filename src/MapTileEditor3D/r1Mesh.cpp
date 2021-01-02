#include "r1Mesh.h"

#include "Application.h"
#include "FileSystem.h"

#include "OpenGLHelper.h"

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
	auto file = FileSystem::OpenJSONFile(path.c_str());

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
	oglh::DeleteVAO(VAO, vertices.id, indices.id);

	delete[] vertices.data;
	delete[] indices.data;
	if (texture.data != nullptr) {
		oglh::DeleteBuffer(texture.id);
		delete[] texture.data;
	}
}

void r1Mesh::GenerateFiles(const aiMesh* mesh)
{
	/*nlohmann::json file;

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

	FileSystem::SaveJSONFile(library_path.c_str(), file);*/
}

void r1Mesh::GenerateBuffers()
{
	// VERTEX ARRAY OBJECT
	oglh::GenVAO(VAO);

	// VERTICES BUFFER
	oglh::GenArrayBuffer(vertices.id, vertices.size, sizeof(float), 3, vertices.data, 0, 3);

	// TEXTURE COORDS BUFFER
	oglh::GenArrayBuffer(texture.id, texture.size, sizeof(float), 2, texture.data, 1, 2);

	// INDICES BUFFER
	oglh::GenElementBuffer(indices.id, indices.size, indices.data);

	oglh::UnBindBuffers();
}
