#pragma once
#include "Resources/Base/Resource.h"
#include "Tools/OpenGL/Buffer.h"

class r1Mesh :
	public Resource
{
	friend class c1Mesh;
public:
	r1Mesh(const uint64_t& uid);
	~r1Mesh();

	void Load() override;
	void Unload() override;

	void GenerateFiles();

private:
	void GenerateBuffers();

//private: TODO: make it private
public:
	unsigned int VAO = 0u;

	Buffer<float> vertices;
	Buffer<float> normals;
	Buffer<unsigned int> indices;
	Buffer<float> texture;

	uint64_t from_model = 0ULL;
	unsigned int tex_i = 0U;
};

