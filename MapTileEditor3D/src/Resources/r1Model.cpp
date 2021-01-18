#include "r1Model.h"

#include "ExternalTools/Assimp/include/Importer.hpp"
#include "ExternalTools/Assimp/include/scene.h"
#include "ExternalTools/Assimp/include/postprocess.h"

#include "Objects/Object.h"

#include "Tools/OpenGL/OpenGLHelper.h"

#include "Tools/System/Logger.h"

r1Model::r1Model(const uint64_t& uid) : Resource(Resource::Type::Model, uid)
{
}

r1Model::~r1Model()
{
}

void r1Model::Load()
{
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate);

	if (scene == nullptr) {
		LOGW("Model %s with path %s not loaded correctly | Error: %s", name.c_str(), path.c_str(), importer.GetErrorString());
		return;
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
	
	node->mTransformation;

	if (node->mMetaData != nullptr) {
		LOG("Metadata of node %s", node->mName.C_Str());
		for (unsigned int i = 0u; i < node->mMetaData->mNumProperties; ++i) {
			auto key = node->mMetaData->mKeys[i];
			auto val = node->mMetaData->mValues[i];
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
				LOG("Metadata with index %i type: double key: %s | value: %d", i, key.C_Str(), *(double*)val.mData);
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

	if (node->mNumMeshes == 1) {
		for (unsigned int i = 0u; i < node->mNumMeshes; ++i) {
			aiMesh* m = scene->mMeshes[node->mMeshes[i]];
			Mesh* mesh = new Mesh();

			oglh::GenVAO(mesh->VAO);

			mesh->vertices.size = m->mNumVertices;
			mesh->vertices.data = new float[mesh->vertices.size * 3];
			memset(mesh->vertices.data, 0.f, sizeof(float) * mesh->vertices.size * 3);

			for (int v = 0; v < m->mNumVertices; ++v) {
				mesh->vertices.data[v*3] = m->mVertices[v].x;
				mesh->vertices.data[v*3+1] = m->mVertices[v].y;
				mesh->vertices.data[v*3+2] = m->mVertices[v].z;
			}

			oglh::GenArrayBuffer(mesh->vertices.id, mesh->vertices.size, sizeof(float), 3, mesh->vertices.data);

			mesh->indices.size = m->mNumFaces * 3;
			mesh->indices.data = new unsigned int[mesh->indices.size];
			memset(mesh->indices.data, 0U, sizeof(unsigned int) * mesh->indices.size);

			for (int f = 0; f < m->mNumFaces; ++f) {
				for (int n = 0; n < m->mFaces[f].mNumIndices; ++n)
					mesh->indices.data[f + n] = m->mFaces[f].mIndices[n];
			}

			oglh::GenElementBuffer(mesh->indices.id, mesh->indices.size, mesh->indices.data);

			n->id_mesh = meshes.size();
			meshes.push_back(mesh);
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

	for (auto i = parent->children.begin(); i != parent->children.end(); ++i) {
		CreateChildren((*i), o);
	}
}
