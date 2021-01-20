#include "r1Model.h"

#include "ExternalTools/Assimp/include/Importer.hpp"
#include "ExternalTools/Assimp/include/scene.h"
#include "ExternalTools/Assimp/include/postprocess.h"

#include "ExternalTools/MathGeoLib/include/Math/Quat.h"

#include "Objects/Object.h"
#include "Objects/Components/c1Mesh.h"
#include "Objects/Components/c1Transform.h"
#include "Resources/r1Mesh.h"

#include "Core/Application.h"
#include "Modules/m1Resources.h"

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

	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate);

	if (scene == nullptr) {
		LOGW("Model %s with path %s not loaded correctly | Error: %s", name.c_str(), path.c_str(), importer.GetErrorString());
		return;
	}

	if (scene->mMetaData != nullptr) {
		LoadMetaData(scene->mMetaData);
	}

	double factor(0.0);
	bool result = scene->mMetaData->Get("UnitScaleFactor", factor);
	if (result == false) {
		LOG("Failed to retrieve  unit scale factor!");
	}
	else {
		LOG("Scale is %lf", factor);
	}

	//Load meshes
	for (int im = 0; im < scene->mNumMeshes; ++im) {
		aiMesh* m = scene->mMeshes[im];
		auto mesh = App->resources->CreateResource<r1Mesh>("", 0ULL, false);

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

		mesh->indices.size = m->mNumFaces * 3;
		mesh->indices.data = new unsigned int[mesh->indices.size];
		memset(mesh->indices.data, 0U, sizeof(unsigned int) * mesh->indices.size);

		for (int f = 0; f < m->mNumFaces; ++f) {
			for (int n = 0; n < m->mFaces[f].mNumIndices; ++n)
				mesh->indices.data[f * m->mFaces[f].mNumIndices + n] = m->mFaces[f].mIndices[n];
		}

		oglh::GenElementBuffer(mesh->indices.id, mesh->indices.size, mesh->indices.data);

		meshes.push_back(mesh);
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
		LOG("Metadata of node %s", node->mName.C_Str());
		LoadMetaData(node->mMetaData);
	}

	if (node->mNumMeshes == 1) {
		for (unsigned int i = 0u; i < node->mNumMeshes; ++i) {
			n->id_mesh = meshes[node->mMeshes[i]]->GetUID();
		}
	}
	else if(node->mNumMeshes > 1) {
		LOGE("Node with more than 1 mesh");
	}

	for (unsigned int i = 0u; i < node->mNumChildren; ++i) {
		Node* child = new Node();
		LoadNode(node->mChildren[i], scene, child);
		child->parent = n;
		n->children.push_back(child);
	}
}

void r1Model::LoadMetaData(aiMetadata* meta)
{
	for (unsigned int i = 0u; i < meta->mNumProperties; ++i) {
		auto key = meta->mKeys[i];
		auto val = meta->mValues[i];
		switch (val.mType)
		{
		case AI_BOOL:
			LOG("Metadata with index %i type: bool key: %s | value: %i", i, key.C_Str(), *(bool*)val.mData);
			break;
		case AI_INT32:
			LOG("Metadata with index %i type: int32_t key: %s | value: %i", i, key.C_Str(), *(int32_t*)val.mData);
			break;
		case AI_UINT64:
			LOG("Metadata with index %i type: uint64_t key: %s | value: %" SDL_PRIu64, i, key.C_Str(), *(uint64_t*)val.mData);
			break;
		case AI_FLOAT:
			LOG("Metadata with index %i type: float key: %s | value: %f", i, key.C_Str(), *(float*)val.mData);
			break;
		case AI_DOUBLE:
			LOG("Metadata with index %i type: double key: %s | value: %lf", i, key.C_Str(), *(double*)val.mData);
			break;
		case AI_AISTRING:
			LOG("Metadata with index %i type: aiString key: %s | value: %s", i, key.C_Str(), (*(aiString*)val.mData).C_Str());
			break;
		case AI_AIVECTOR3D: {
			aiVector3D value = *(aiVector3D*)val.mData;
			LOG("Metadata with index %i type: aiVector3D key: %s | value: (%f, %f, %f)", i, key.C_Str(), value.x, value.y, value.z);
		}	break;
		default:
			LOG("Metadata with index %i type not handled in the switch | Type: %i", i, val.mType);
			break;
		}
	}
}

void r1Model::CreateObject(Object* r)
{
	if (r == nullptr)
		return;

	Object* parent = new Object(r);
	parent->SetName(name.c_str());

	for (auto i = root->children.begin(); i != root->children.end(); ++i)
		CreateChildren(*i, parent);
}

void r1Model::CreateChildren(r1Model::Node* parent, Object* r)
{
	Object* o = new Object(r);

	o->SetName(parent->name.c_str());

	o->transform->SetLocalMatrix(parent->transform);

	if (parent->id_mesh != -1) {
		auto mesh = o->CreateComponent<c1Mesh>();
		mesh->SetMesh(parent->id_mesh);
	}

	for (auto i = parent->children.begin(); i != parent->children.end(); ++i) {
		CreateChildren((*i), o);
	}
}
