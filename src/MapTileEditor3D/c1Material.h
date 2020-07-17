#pragma once
#include "Component.h"

#include "ExternalTools/MathGeoLib/include/Math/float3.h"

class r1Shader;
class r1Texture;

class c1Material : public Component
{
public:
	c1Material(Object* obj);
	~c1Material();

	void BindTex();

	void UnBindTex();

	void OnInspector() override;

	void SetTexture(const uint64_t& id);
	void SetTexture(const r1Texture* tex);
	void SetTexture(const char* name_tex);

private:
	void ChooseTextureWindow(bool& choose_texture);

public:
	r1Shader* shader = nullptr;
private:
	float3 color = float3::one;

	uint64_t tex = 0ULL;
};

