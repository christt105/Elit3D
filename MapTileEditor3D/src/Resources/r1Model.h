#pragma once
#include "Resources/Base/Resource.h"
#include <vector>
#include "ExternalTools/JSON/json.hpp"

#include "Tools/OpenGL/Buffer.h"

struct aiNode;
struct aiScene;
class Object;

class r1Model :
	public Resource
{

	struct Node {
		std::string name;

		int id_mesh = -1;

		std::vector<Node*> children;
		Node* parent = nullptr;
	};

	struct Mesh {
		unsigned int VAO = 0u;
		Buffer<float> vertices;
		Buffer<unsigned int> indices;
		Buffer<float> texture;
	};
public:
	r1Model(const uint64_t& uid);
	~r1Model();

	void Load() override;
	void LoadNode(aiNode* root, const aiScene* scene, Node* parent);

	void CreateObject(Object* root);

	void CreateChildren(r1Model::Node* parent, Object* r);

	std::vector<Mesh*> meshes;
	Node* root = nullptr;
};

