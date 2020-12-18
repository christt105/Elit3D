#pragma once
#include "Resource.h"

#include "OpenGLHelper.h"

struct aiTexture;

class r1Texture :
	public Resource
{
	friend class c1Material;
public:
	r1Texture(const uint64_t& uid);
	~r1Texture();

	void Load() override;
	void LoadVars() override;
	void Unload() override;

	unsigned int GetBufferID();

	unsigned int GetWidth();
	unsigned int GetHeight();

	void OnInspector() override;

	void GenerateFiles(const aiTexture* texture);

	void Bind();
	void Unbind();

	void Edit(int row, int col, int r, int g, int b);

public:
	bool tileset = false;

private:
	unsigned int id = 0U;

	int width = 0U;
	int height = 0U;
	int channels = 0U;
	
	oglh::Wrap wrap = oglh::Wrap::Repeat;
	oglh::Filter filter = oglh::Filter::Nearest;
};

