#pragma once
#include <SDL_stdinc.h>
#include "Component.h"

#include "m1Resources.h"

class c1Material;
class r1Mesh;

class c1Mesh : public Component
{
	friend class m1Objects;
	friend class m1Scene;
public:
	c1Mesh(Object* obj);
	~c1Mesh();

	void Update() override;

	void SetMesh(const uint64_t& id);
	void SetEMesh(m1Resources::EResourceType res);

	void OnInspector() override;

private:
	union {
		Uint64 mesh = 0ULL;
		r1Mesh* emesh;
	};

	bool is_engine_mesh = false;
	
	c1Material* material = nullptr;
};

