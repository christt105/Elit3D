#pragma once
#include <SDL_stdinc.h>
#include "Objects/Components/Base/Component.h"

#include "Modules/m1Resources.h"

class c1Material;
class r1Mesh;

class c1Mesh : public Component
{
	friend class m1Objects;
	friend class m1MapEditor;
	friend class r1Map;
public:
	c1Mesh(Object* obj);
	~c1Mesh();

	void Update() override;

	void SetMesh(const uint64_t& id);
	void SetEMesh(m1Resources::EResourceType res);

	void OnInspector() override;

	inline uint64_t GetMesh() const { return mesh; }

	nlohmann::json	Parse() override;
	void			Unparse(const nlohmann::json& node) override;

private:
	union {
		Uint64 mesh = 0ULL;
		r1Mesh* emesh;
	};

	bool is_engine_mesh = false;
	
	c1Material* material = nullptr;
};

