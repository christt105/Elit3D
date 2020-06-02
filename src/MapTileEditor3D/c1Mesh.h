#pragma once
#include <SDL_stdinc.h>
#include "Component.h"

class c1Material;

class c1Mesh : public Component
{
	friend class m1Objects;
public:
	c1Mesh(Object* obj);
	~c1Mesh();

	void Update() override;

	void SetMesh(const uint64_t& id);

	void OnInspector() override;

private:
	Uint64 mesh = 0ULL;
	
	c1Material* material = nullptr;
};

