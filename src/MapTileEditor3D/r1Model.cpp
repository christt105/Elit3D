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
#include "ExternalTools/Assimp/include/metadata.h"

#include "ExternalTools/MathGeoLib/include/Math/Quat.h"

#include "Logger.h"

#include "ExternalTools/mmgr/mmgr.h"

#pragma comment(lib, "ExternalTools/Assimp/libx86/assimp-vc141-mtd.lib")

r1Model::r1Model(const uint64_t& id) : Resource(Resource::Type::Model, id)
{
}

r1Model::~r1Model()
{
}

//void r1Model::GenerateFiles()
//{
	//Assimp::Importer importer;
	//const aiScene* scene = importer.ReadFile(path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_FlipUVs);
	//
	//if (scene != nullptr && scene->mRootNode != nullptr) {
	//	std::vector<uint64_t> meshes;
	//	std::vector<uint64_t> textures;

	//	double factor = 0.0;
	//	if (scene->mMetaData->Get("UnitScaleFactor", factor)) {
	//		LOG("SCALE FACTOR: %f", factor);
	//	}

	//	for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
	//		r1Mesh* m = App->resources->CreateResource<r1Mesh>(path.c_str(), 0Ui64, false);
	//		m->GenerateFiles(scene->mMeshes[i]);
	//		meshes.push_back(m->GetUID());
	//	}

	//	for (unsigned int i = 0; i < scene->mNumMaterials; ++i) { //TODO
	//		const aiMaterial* mat = scene->mMaterials[i];
	//		unsigned int n_tex = mat->GetTextureCount(aiTextureType::aiTextureType_DIFFUSE);
	//		for (unsigned int i = 0; i < n_tex; ++i) {
	//			aiString path;
	//			if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &path) == aiReturn_SUCCESS) {
	//				if (FileSystem::IsFileInFolder(FileSystem::GetNameFile(path.C_Str(), true).c_str(), "Assets/", true)) {
	//					auto res = App->resources->Get(App->resources->FindByName(FileSystem::GetNameFile(path.C_Str(), false).c_str()));
	//					if (res != nullptr)
	//						textures.push_back(res->GetUID());
	//				}
	//			}
	//		}

	//		/*r1Texture* m = App->resources->CreateResource<r1Texture>(assets_path.c_str());
	//		m->GenerateFiles(scene->mTextures[i]);
	//		textures.push_back(m->GetUID());*/
	//	}

	//	for (unsigned int i = 0; i < scene->mNumTextures; ++i) { //TODO
	//		LOGW("Textures import from FBX not done already...");
	//		/*r1Texture* m = App->resources->CreateResource<r1Texture>(assets_path.c_str());
	//		m->GenerateFiles(scene->mTextures[i]);
	//		textures.push_back(m->GetUID());*/
	//	}

	//	Node object = FillNodeHierarchy(scene->mRootNode, meshes, textures);
	//	nlohmann::json model;

	//	for (auto i = meshes.begin(); i != meshes.end(); ++i) {
	//		model["Meshes"].push_back(*i);
	//	}

	//	for (auto i = textures.begin(); i != textures.end(); ++i) {
	//		model["Textures"].push_back(*i);
	//	}

	//	model["Hierarchy"] = object.Parse();

	//	FileSystem::SaveJSONFile(library_path.c_str(), model);
	//}
//}

Object* r1Model::CreateObject()
{
	Object* obj = App->objects->CreateEmptyObject();
	obj->SetName(name.c_str());

	/*nlohmann::json jobj = FileSystem::OpenJSONFile(library_path.c_str());

	CreateChildren(jobj["Hierarchy"], obj);*/
	
	return obj;
}

void r1Model::CreateChildren(nlohmann::json& jobj, Object* parent)
{
	for (auto i = jobj["Children"].begin(); i != jobj["Children"].end(); ++i) {
		Object* child = parent->CreateChild();
		child->SetName((*i).value("name", "Object").c_str());

		float3 pos((*i)["position"][0], (*i)["position"][1], (*i)["position"][2]);
		float3 scale((*i)["scale"][0], (*i)["scale"][1], (*i)["scale"][2]);
		Quat rot((*i)["rotation"]["x"], (*i)["rotation"]["y"], (*i)["rotation"]["z"], (*i)["rotation"]["w"]);
		child->transform->SetMatrix(float4x4::FromTRS(pos, rot, scale));

		if ((*i)["meshID"] != 0) {
			c1Mesh* mesh = child->CreateComponent<c1Mesh>();
			mesh->SetMesh((*i)["meshID"]);
			if ((*i)["meshID"] != 0) {
				//c1Mesh* mesh = child-><c1Mesh>();
				//mesh->SetMesh((*i)["meshID"]);
			}
		}

		if ((*i).find("Children") != (*i).end())
			CreateChildren(*i, child);
	}
}

void r1Model::Load()
{
	//nlohmann::json model = FileSystem::OpenJSONFile(library_path.c_str());
}

void r1Model::Unload()
{
}

r1Model::Node r1Model::FillNodeHierarchy(const aiNode* parent, const std::vector<uint64_t>& mesh_ids, const std::vector<uint64_t>& tex_ids)
{
	Node ret;
	ret.name.assign(parent->mName.C_Str());

	aiVector3D pos, scale;
	aiQuaternion rot;
	parent->mTransformation.Decompose(scale, rot, pos);
	ret.pos[0] = pos.x;
	ret.pos[1] = pos.y;
	ret.pos[2] = pos.z;

	ret.rot[0] = rot.w;
	ret.rot[1] = rot.x;
	ret.rot[2] = rot.y;
	ret.rot[3] = rot.z;
	
	float unit_scale = 1.f;
	if (scale.Length() >= 99.9f)
		unit_scale = 100.f;

	ret.scale[0] = scale.x / unit_scale;
	ret.scale[1] = scale.y / unit_scale;
	ret.scale[2] = scale.z / unit_scale;

	if (parent->mNumMeshes == 1) {
		ret.mesh = mesh_ids[parent->mMeshes[0]];
		ret.texture = 0ULL;//tex_ids[parent->mMeshes[0]];
	}
	else {
		for (unsigned int i = 0u; i < parent->mNumMeshes; ++i) {
			Node mesh;
			mesh.mesh = mesh_ids[parent->mMeshes[i]];
			ret.texture = 0ULL;//tex_ids[parent->mMeshes[i]];
			ret.children.push_back(mesh);
		}
	}

	for (unsigned int i = 0u; i < parent->mNumChildren; ++i) {
		ret.children.push_back(FillNodeHierarchy(parent->mChildren[i], mesh_ids, tex_ids));
	}

	return ret;
}

nlohmann::json r1Model::Node::Parse()
{
	nlohmann::json node;

	if (mesh != 0ull)
		node["meshID"] = mesh;
	if (texture != 0ull)
		node["texID"] = texture;

	node["name"] = name;

	for (int i = 0; i < 3; ++i) {
		node["position"].push_back(pos[i]);
		node["scale"].push_back(scale[i]);
	}
	node["rotation"]["w"] = rot[0];
	node["rotation"]["x"] = rot[1];
	node["rotation"]["y"] = rot[2];
	node["rotation"]["z"] = rot[3];

	for (auto i = children.begin(); i != children.end(); ++i)
		node["Children"].push_back((*i).Parse());

	return node;
}
