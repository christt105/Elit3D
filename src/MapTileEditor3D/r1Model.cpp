#include "r1Model.h"

#include "Application.h"
#include "m1Resources.h"
#include "m1Objects.h"
#include "FileSystem.h"

#include "Object.h"
#include "c1Mesh.h"
#include "c1Transform.h"

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
			r1Mesh* m = App->resources->CreateResource<r1Mesh>(assets_path.c_str());
			m->GenerateFiles(scene->mMeshes[i]);
			meshes.push_back(m->GetUID());
		}

		for (unsigned int i = 0; i < scene->mNumTextures; ++i) {
			r1Texture* m = App->resources->CreateResource<r1Texture>(assets_path.c_str());
			m->GenerateFiles(scene->mTextures[i]);
			textures.push_back(m->GetUID());
		}

		Node object = FillNodeHierarchy(scene->mRootNode, meshes);
		nlohmann::json model;

		for (auto i = meshes.begin(); i != meshes.end(); ++i) {
			model["Meshes"].push_back(*i);
		}

		for (auto i = textures.begin(); i != textures.end(); ++i) {
			model["Textures"].push_back(*i);
		}

		model["Hierarchy"] = object.Parse();

		App->file_system->SaveJSONFile(library_path.c_str(), model);
	}
}

void r1Model::LoadLibrary()
{
	nlohmann::json model = App->file_system->OpenJSONFile(library_path.c_str());

	for (auto i = model["Meshes"].begin(); i != model["Meshes"].end(); ++i) {
		App->resources->CreateResource<r1Mesh>("", *i);
	}
}

Object* r1Model::CreateObject()
{
	Object* obj = App->objects->CreateEmptyObject();
	obj->SetName(name.c_str());

	nlohmann::json jobj = App->file_system->OpenJSONFile(library_path.c_str());

	CreateChildren(jobj["Hierarchy"], obj);

	return obj;
}

void r1Model::CreateChildren(nlohmann::json& jobj, Object* parent)
{
	for (auto i = jobj["Children"].begin(); i != jobj["Children"].end(); ++i) {
		Object* child = parent->CreateChild();
		child->SetName((*i).value("name", "Object").c_str());

		if ((*i)["meshID"] != 0) {
			c1Mesh* mesh = child->CreateComponent<c1Mesh>();
			mesh->SetMesh((*i)["meshID"]);
		}

		if ((*i).find("Children") != (*i).end())
			CreateChildren(*i, child);
	}
}

void r1Model::Load()
{
	nlohmann::json model = App->file_system->OpenJSONFile(library_path.c_str());

	
}

void r1Model::Unload()
{
}

r1Model::Node r1Model::FillNodeHierarchy(const aiNode* parent, const std::vector<uint64_t>& ids)
{
	Node ret;
	ret.name.assign(parent->mName.C_Str());

	ret.transform[0] = parent->mTransformation.a1;
	ret.transform[1] = parent->mTransformation.a2;
	ret.transform[2] = parent->mTransformation.a3;
	ret.transform[3] = parent->mTransformation.a4;
	ret.transform[4] = parent->mTransformation.b2;
	ret.transform[5] = parent->mTransformation.b1;
	ret.transform[6] = parent->mTransformation.b3;
	ret.transform[7] = parent->mTransformation.b4;
	ret.transform[8] = parent->mTransformation.c1;
	ret.transform[9] = parent->mTransformation.c2;
	ret.transform[10] = parent->mTransformation.c3;
	ret.transform[11] = parent->mTransformation.c4;
	ret.transform[12] = parent->mTransformation.d1;
	ret.transform[13] = parent->mTransformation.d2;
	ret.transform[14] = parent->mTransformation.d3;
	ret.transform[15] = parent->mTransformation.d4;

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

	for (int i = 0; i < 16; ++i)
		node["transformation"].push_back(transform[i]);

	for (auto i = children.begin(); i != children.end(); ++i)
		node["Children"].push_back((*i).Parse());

	return node;
}
