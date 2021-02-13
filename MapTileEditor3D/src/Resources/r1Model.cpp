#include "r1Model.h"

#include "ExternalTools/Assimp/include/Importer.hpp"
#include "ExternalTools/Assimp/include/scene.h"
#include "ExternalTools/Assimp/include/postprocess.h"

#include "ExternalTools/MathGeoLib/include/Math/Quat.h"

#include "Modules/m1Objects.h"
#include "Objects/Object.h"
#include "Objects/Components/c1Mesh.h"
#include "Objects/Components/c1Material.h"
#include "Objects/Components/c1Transform.h"
#include "Resources/r1Mesh.h"

#include "Core/Application.h"
#include "Modules/m1Resources.h"

#include "Tools/FileSystem.h"

#include "Tools/OpenGL/OpenGLHelper.h"

#include "Tools/System/Logger.h"

r1Model::r1Model(const uint64_t& uid) : Resource(Resource::Type::Model, uid)
{
}

r1Model::~r1Model()
{
	if (root != nullptr)
		delete root;
}

void r1Model::Load()
{
	Assimp::Importer importer;
	
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GlobalScale);

	if (scene == nullptr) {
		LOGW("Model %s with path %s not loaded correctly | Error: %s", name.c_str(), path.c_str(), importer.GetErrorString());
		return;
	}

	if (scene->mMetaData != nullptr) {
		LoadMetaData(scene->mMetaData);
	}

	nlohmann::json meta = FileSystem::OpenJSONFile((path + ".meta").c_str());

	if (meta.is_null()) {
		LOGW("meta file of resource %s was not found", path.c_str());
		return;
	}

	meta = meta["properties"];

	//Load meshes
	for (int im = 0; im < scene->mNumMeshes; ++im) {
		aiMesh* m = scene->mMeshes[im];

		nlohmann::json jmesh = meta["meshes"][im];

		auto mesh = App->resources->CreateResource<r1Mesh>("", jmesh.value("uid", 0ULL), false);
		mesh->from_model = uid;

		if (m->mMaterialIndex >= 0) {
			mesh->tex_i = m->mMaterialIndex;
		}

		oglh::GenVAO(mesh->VAO);
		
		mesh->vertices.size = m->mNumVertices;
		mesh->vertices.data = new float[mesh->vertices.size * 3];
		memset(mesh->vertices.data, 0.f, sizeof(float) * mesh->vertices.size * 3);

		for (int v = 0; v < m->mNumVertices; ++v) {
			mesh->vertices.data[v * 3] = m->mVertices[v].x;
			mesh->vertices.data[v * 3 + 1] = m->mVertices[v].y;
			mesh->vertices.data[v * 3 + 2] = m->mVertices[v].z;
		}

		oglh::GenArrayBuffer(mesh->vertices.id, mesh->vertices.size, sizeof(float), 3, mesh->vertices.data, 0, 3);

		if (m->HasNormals()) {
			mesh->normals.size = m->mNumVertices;
			mesh->normals.data = new float[mesh->normals.size * 3];
			memset(mesh->normals.data, 0.f, sizeof(float) * mesh->normals.size * 3);

			for (int n = 0; n < m->mNumVertices; ++n) {
				mesh->normals.data[n * 3] = m->mNormals[n].x;
				mesh->normals.data[n * 3 + 1] = m->mNormals[n].y;
				mesh->normals.data[n * 3 + 2] = m->mNormals[n].z;
			}

			oglh::GenArrayBuffer(mesh->normals.id, mesh->normals.size, sizeof(float), 3, mesh->normals.data, 2, 3);
		}

		mesh->indices.size = m->mNumFaces * 3;
		mesh->indices.data = new unsigned int[mesh->indices.size];
		memset(mesh->indices.data, 0U, sizeof(unsigned int) * mesh->indices.size);

		for (int f = 0; f < m->mNumFaces; ++f) {
			for (int n = 0; n < m->mFaces[f].mNumIndices; ++n)
				mesh->indices.data[f * m->mFaces[f].mNumIndices + n] = m->mFaces[f].mIndices[n];
		}

		oglh::GenElementBuffer(mesh->indices.id, mesh->indices.size, mesh->indices.data);

		if (m->HasTextureCoords(0)) {
			mesh->texture.size = m->mNumVertices;
			mesh->texture.data = new float[mesh->texture.size * 2];
			memset(mesh->texture.data, 0.f, sizeof(float) * mesh->texture.size * 2);

			for (int n = 0; n < m->mNumVertices; ++n) {
				mesh->texture.data[n * 2] = m->mTextureCoords[0][n].x;
				mesh->texture.data[n * 2 + 1] = m->mTextureCoords[0][n].y;
			}

			oglh::GenArrayBuffer(mesh->texture.id, mesh->texture.size, sizeof(float), 2, mesh->texture.data, 1, 2);
		}

		meshes.push_back(mesh);
	}

	for (int it = 0; it < scene->mNumMaterials; ++it) {
		aiMaterial const * mat = scene->mMaterials[it];

		unsigned int ntex = mat->GetTextureCount(aiTextureType::aiTextureType_DIFFUSE);
		for (unsigned int i = 0; i < ntex; ++i) {
			aiString p;
			if (mat->GetTexture(aiTextureType::aiTextureType_DIFFUSE, i, &p) == aiReturn::aiReturn_FAILURE) {
				LOGE("Error get diffuse texture from assimp");
				continue;
			}
			uint64_t t_uid = 0ULL;
			if (FileSystem::Exists(p.C_Str())) {
				t_uid = App->resources->FindByPath(p.C_Str());
				if (t_uid == 0ULL)
					t_uid = App->resources->FindByName(FileSystem::GetNameFile(p.C_Str()).c_str());
			}
			else {
				t_uid = App->resources->FindByName(FileSystem::GetNameFile(p.C_Str()).c_str());
			}
			materials.push_back(t_uid);
		}
	}

	root = new Node();
	root->name = "root";
	for (int i = 0; i < scene->mRootNode->mNumChildren; ++i) {
		Node* c = new Node();
		LoadNode(scene->mRootNode->mChildren[i], scene, c);
		c->parent = root;
		root->children.push_back(c);
	}
}

void r1Model::LoadNode(aiNode* node, const aiScene* scene, Node* n)
{
	n->name = node->mName.C_Str();
	
	aiVector3D pos, scale; aiQuaternion rot;
	node->mTransformation.Decompose(scale, rot, pos);
	n->transform = float4x4::FromTRS({ pos.x, pos.y, pos.z }, Quat(rot.x, rot.y, rot.z, rot.w), { scale.x, scale.y, scale.z });

	if (node->mMetaData != nullptr) {
		LoadMetaData(node->mMetaData);
	}

	if (node->mNumMeshes == 1) {
		for (unsigned int i = 0u; i < node->mNumMeshes; ++i) {
			n->id_mesh = meshes[node->mMeshes[i]]->GetUID();
			int m = scene->mMeshes[node->mMeshes[i]]->mMaterialIndex;
			if (m >= 0 && m < materials.size())
				n->id_tex = materials[m];
		}
	}
	else if(node->mNumMeshes > 1) {
		LOGE("TODO: Node with more than 1 mesh");
	}

	for (unsigned int i = 0u; i < node->mNumChildren; ++i) {
		Node* child = new Node();
		LoadNode(node->mChildren[i], scene, child);
		child->parent = n;
		n->children.push_back(child);
	}
}

void r1Model::CreateHierarchy(nlohmann::json& parent, aiNode* node)
{
	parent["name"] = node->mName.C_Str();
	
	if (node->mNumMeshes > 0) {
		if (node->mNumMeshes == 1) {
			parent["id_mesh"] = node->mMeshes[0];
		}
		else {
			//TODO: create separated object for every mesh
		}
	}

	for (int i = 0; i < node->mNumChildren; ++i) {
		nlohmann::json child;
		CreateHierarchy(child, node->mChildren[i]);
		parent["children"].push_back(child);
	}
}

void r1Model::GenerateFiles()
{
	Assimp::Importer importer;

	nlohmann::json meta = FileSystem::OpenJSONFile((path + ".meta").c_str());

	if (meta.is_null()) {
		LOGW("meta file of resource %s was not found", path.c_str());
		return;
	}

	auto mprop = meta["properties"];

	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate);

	if (scene == nullptr) {
		LOGW("Model %s with path %s not loaded correctly | Error: %s", name.c_str(), path.c_str(), importer.GetErrorString());
		return;
	}

	if (scene->mMetaData != nullptr) {
		LoadMetaData(scene->mMetaData);
	}

	for (int im = 0; im < scene->mNumMeshes; ++im) {
		nlohmann::json jmesh;
		jmesh["uid"] = Random::RandomGUID();
		jmesh["index"] = im;

		mprop["meshes"].push_back(jmesh);
	}

	for (int it = 0; it < scene->mNumMaterials; ++it) {
		LOGN("Material %s", scene->mMaterials[it]->GetName().C_Str());
		for (int s = 0; s < scene->mMaterials[it]->GetTextureCount(aiTextureType::aiTextureType_SPECULAR); ++s) {
			//TODO: Load more texture types
		}
		for (int d = 0; d < scene->mMaterials[it]->GetTextureCount(aiTextureType::aiTextureType_DIFFUSE); ++d) {
			aiString p;
			if (scene->mMaterials[it]->GetTexture(aiTextureType::aiTextureType_DIFFUSE, d, &p) == aiReturn::aiReturn_FAILURE) {
				LOGE("GetTexture Failed");
				continue;
			}
			LOG("Texture %s", p.C_Str());
		}
	}

	for (int it = 0; it < scene->mNumTextures; ++it) {
		LOGN("Texture %s", scene->mTextures[it]->mFilename.C_Str());
	}

	CreateHierarchy(mprop["root"], scene->mRootNode);

	meta["properties"] = mprop;

	FileSystem::SaveJSONFile((path + ".meta").c_str(), meta);
}

void r1Model::UpdateFiles()
{
	LOGNE("TODO: fbx Updated");
}

void r1Model::LoadMetaData(aiMetadata* meta)
{
	for (unsigned int i = 0u; i < meta->mNumProperties; ++i) {
		auto key = meta->mKeys[i];
		auto val = meta->mValues[i];
		switch (val.mType)
		{
		case AI_BOOL:
			//LOG("Metadata with index %i type: bool key: %s | value: %i", i, key.C_Str(), *(bool*)val.mData);
			break;
		case AI_INT32:
			//LOG("Metadata with index %i type: int32_t key: %s | value: %i", i, key.C_Str(), *(int32_t*)val.mData);
			break;
		case AI_UINT64:
			//LOG("Metadata with index %i type: uint64_t key: %s | value: %" SDL_PRIu64, i, key.C_Str(), *(uint64_t*)val.mData);
			break;
		case AI_FLOAT:
			//LOG("Metadata with index %i type: float key: %s | value: %f", i, key.C_Str(), *(float*)val.mData);
			break;
		case AI_DOUBLE:
			//LOG("Metadata with index %i type: double key: %s | value: %lf", i, key.C_Str(), *(double*)val.mData);
			break;
		case AI_AISTRING:
			//LOG("Metadata with index %i type: aiString key: %s | value: %s", i, key.C_Str(), (*(aiString*)val.mData).C_Str());
			break;
		case AI_AIVECTOR3D: {
			aiVector3D value = *(aiVector3D*)val.mData;
			//LOG("Metadata with index %i type: aiVector3D key: %s | value: (%f, %f, %f)", i, key.C_Str(), value.x, value.y, value.z);
		}	break;
		default:
			//LOG("Metadata with index %i type not handled in the switch | Type: %i", i, val.mType);
			break;
		}
	}
}

void r1Model::CreateObject(Object* r)
{
	if (r == nullptr)
		return;

	Object* parent = App->objects->CreateEmptyObject(r, name.c_str());

	for (auto i = root->children.begin(); i != root->children.end(); ++i)
		CreateChildren(*i, parent);
}

void r1Model::OnInspector()
{
	Resource::OnInspector();

	ImGui::Separator();


}

void r1Model::CreateChildren(r1Model::Node* parent, Object* r)
{
	Object* o = App->objects->CreateEmptyObject(r);

	o->SetName(parent->name.c_str());

	o->transform->SetLocalMatrix(parent->transform);

	if (parent->id_mesh != 0ULL) {
		auto mesh = o->CreateComponent<c1Mesh>();
		mesh->SetMesh(parent->id_mesh);

		if (parent->id_tex != 0ULL) {
			auto mat = o->GetComponent<c1Material>();
			mat->SetTexture(parent->id_tex);
		}
	}

	for (auto i = parent->children.begin(); i != parent->children.end(); ++i) {
		CreateChildren((*i), o);
	}
}
