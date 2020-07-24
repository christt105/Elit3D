#pragma once

#include "int2.h"

class Viewport
{
	friend class m1Render3D;
private:
	Viewport();
	~Viewport();

	void Clear();

public:
	void Update();
	void UpdateSize(int x, int y);
	void Blit() const;

	unsigned int GetTexture() const;

	void Begin() const;
	void End() const;


private:
	int smaa = 8;
	int2 size;

	enum FBOIDS
	{
		FBO,
		FBO_MS,
		TEXTURE,
		TEXTURE_MS,
		RBO,
		RBO_MS
	};
	unsigned int ID[6];

	float color[3] = { .2f, .2f, .2f };

	//Camera* camera = nullptr;
};

