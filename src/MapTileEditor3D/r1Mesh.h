#pragma once
#include "Resource.h"
#include "Buffer.h"

struct aiMesh;

class r1Mesh :
	public Resource
{
	friend class c1Mesh;
public:
	r1Mesh(const uint64_t& uid);
	~r1Mesh();

	void Load() override;
	void Unload() override;

	void GenerateFiles(const aiMesh* mesh);

private:
	void GenerateBuffers();

//private: TODO: make it private
public:
	unsigned int VAO = 0u;

	Buffer<float> vertices;
	Buffer<unsigned int> indices;
	Buffer<float> texture;
};

