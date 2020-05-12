#pragma once
#include "Resource.h"
#include <vector>
#include "ExternalTools/JSON/json.hpp"

struct aiNode;

class r1Model :
	public Resource
{
	struct Node {
		std::string name;
		std::vector<Node> children;
		uint64_t mesh = 0ull;

		nlohmann::json Parse();
	};
public:
	r1Model(const uint64_t& uid);
	~r1Model();

	void GenerateFiles() override;

private:
	Node FillNodeHierarchy(const aiNode* parent, const std::vector<uint64_t>& ids);
};

