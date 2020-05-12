#include "r1Model.h"

#include "Application.h"
#include "m1Resources.h"
#include "FileSystem.h"

#include "r1Mesh.h"
#include "r1Texture.h"

#include "ExternalTools/Assimp/include/Importer.hpp"
#include "ExternalTools/Assimp/include/scene.h"
#include "ExternalTools/Assimp/include/postprocess.h"
#include "ExternalTools/Assimp/include/cimport.h"

#include "Logger.h"

#include "ExternalTools/mmgr/mmgr.h"

#pragma comment(lib, "ExternalTools/Assimp/libx86/assimp-vc141-mtd.lib")

r1Model::r1Model(const uint64_t& id) : Resource(Resource::Type::Model, id)
{
}

r1Model::~r1Model()
{
}

void r1Model::GenerateFiles()
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(assets_path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
	
	if (scene != nullptr && scene->mRootNode != nullptr) {
		std::vector<uint64_t> meshes;
		std::vector<uint64_t> textures;

		for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
			r1Mesh* m = (r1Mesh*)App->resources->CreateResource(Resource::Type::Mesh, assets_path.c_str());
			m->GenerateFiles(scene->mMeshes[i]);
			meshes.push_back(m->GetUID());
		}

		for (unsigned int i = 0; i < scene->mNumTextures; ++i) {
			r1Texture* m = (r1Texture*)App->resources->CreateResource(Resource::Type::Texture, assets_path.c_str());
			m->GenerateFiles(scene->mTextures[i]);
			textures.push_back(m->GetUID());
		}

		Node object = FillNodeHierarchy(scene->mRootNode, meshes);
		nlohmann::json model;
		for (auto i = object.children.begin(); i != object.children.end(); ++i) {
			model.push_back((*i).Parse());
		}

		App->file_system->SaveJSONFile(library_path.c_str(), model);
	}
}

r1Model::Node r1Model::FillNodeHierarchy(const aiNode* parent, const std::vector<uint64_t>& ids)
{
	Node ret;
	ret.name.assign(parent->mName.C_Str());

	if (parent->mNumMeshes == 1)
		ret.mesh = ids[parent->mMeshes[0]];
	else
		for (unsigned int i = 0u; i < parent->mNumMeshes; ++i) {
			Node mesh;
			mesh.mesh = ids[parent->mMeshes[i]];
			ret.children.push_back(mesh);
		}

	for (unsigned int i = 0u; i < parent->mNumChildren; ++i) {
		ret.children.push_back(FillNodeHierarchy(parent->mChildren[i], ids));
	}

	return ret;
}

nlohmann::json r1Model::Node::Parse()
{
	nlohmann::json node;

	node["meshID"] = mesh;
	node["name"] = name;

	for (auto i = children.begin(); i != children.end(); ++i)
		node.push_back((*i).Parse());

	return node;
}
