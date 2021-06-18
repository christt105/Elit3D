#pragma once
#include "Resources/Base/Resource.h"
#include <vector>
#include "ExternalTools/JSON/json.hpp"

#include "ExternalTools/MathGeoLib/include/Math/float4x4.h"

#include "Tools/OpenGL/Buffer.h"

struct aiNode;
struct aiScene;
class Object;
class r1Mesh;
struct aiMetadata;

class r1Model :
	public Resource
{
public:
	struct Node {
		~Node() {
			for (auto i = children.begin(); i != children.end(); ++i)
				delete* i;
		}
		std::string name;

		uint64_t id_mesh = 0ULL;
		uint64_t id_tex = 0ULL;

		float4x4 transform = float4x4::identity;

		std::vector<Node*> children;
		Node* parent = nullptr;
	};

public:
	r1Model(const uint64_t& uid);
	~r1Model();

	void Unload() override;
	void Load() override;
	void LoadVars() override;

	void GenerateFiles() override;
	void UpdateFiles() override;

	void CreateObject(Object* root);

	void OnInspector() override;

private:
	void LoadMetaData(aiMetadata* meta);
	void LoadNode(aiNode* root, const aiScene* scene, Node* parent);

	void CreateHierarchy(nlohmann::json& parent, aiNode* node);

	void CreateChildren(r1Model::Node* parent, Object* r);

public:
	std::vector<r1Mesh*> meshes;
	std::vector<uint64_t> materials;
	Node* root = nullptr;

	bool isTerrain = false;
};

