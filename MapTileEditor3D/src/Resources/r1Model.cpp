#include "r1Model.h"

#include "ExternalTools/Assimp/include/Importer.hpp"
#include "ExternalTools/Assimp/include/scene.h"

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

	const aiScene* scene = importer.ReadFile(path, NULL);

	if (scene == nullptr) {
		LOGW("Model %s with path %s not loaded correctly | Error: %s", name.c_str(), path.c_str(), importer.GetErrorString());
		return;
	}

	LoadNode(scene->mRootNode, scene);
}

void r1Model::LoadNode(aiNode* node, const aiScene* scene)
{
	LOG("Node Name: %s", node->mName.C_Str());
	
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

	for (unsigned int i = 0u; i < node->mNumMeshes; ++i) {
		scene->mMeshes[node->mMeshes[i]];
	}

	for (unsigned int i = 0u; i < node->mNumChildren; ++i) {
		LoadNode(node->mChildren[i], scene);
	}
}
