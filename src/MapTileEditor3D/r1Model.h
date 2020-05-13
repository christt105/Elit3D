#pragma once
#include "Resource.h"
#include <vector>
#include "ExternalTools/JSON/json.hpp"

struct aiNode;
class Object;

class r1Model :
	public Resource
{
	struct Node {
		std::string name;
		std::vector<Node> children;

		float pos[3];
		float scale[3];
		float rot[4];
		uint64_t mesh = 0ull;

		nlohmann::json Parse();
	};
public:
	r1Model(const uint64_t& uid);
	~r1Model();

	void GenerateFiles() override;

	void LoadLibrary() override;

	Object* CreateObject();

	void Load() override;
	void Unload() override;

private:
	void CreateChildren(nlohmann::json& jobj, Object* obj);

private:
	Node FillNodeHierarchy(const aiNode* parent, const std::vector<uint64_t>& ids);
};

