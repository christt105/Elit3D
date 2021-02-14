#pragma once
#include "Modules/Base/Module.h"

#include "ExternalTools/MathGeoLib/include/Math/float2.h"

class Camera;

class m1Camera3D :
	public Module
{
	friend class p1Configuration;
public:
	m1Camera3D(bool start_enabled = true);
	~m1Camera3D();

	bool Init(const nlohmann::json& node) override;

	void SetFov();

	UpdateStatus PreUpdate() override;
	bool CleanUp() override;

	Camera* CreateCamera(const char* id);
	void UpdateShaders(Camera* cam);

	void Save(nlohmann::json& node) override;
	void Load(const nlohmann::json& node) override;

private:
	std::vector<Camera*> cameras;
	Camera* scene = nullptr;
};

