#pragma once
#include "Resources/Base/Resource.h"
#include <vector>
#include "ExternalTools/JSON/json.hpp"

struct aiNode;
struct aiScene;

class r1Model :
	public Resource
{
public:
	r1Model(const uint64_t& uid);
	~r1Model();

	void Load() override;
	void LoadNode(aiNode* root, const aiScene* scene);
};

