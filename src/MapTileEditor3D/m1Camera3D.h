#pragma once
#include "Module.h"

#include "ExternalTools/MathGeoLib/include/Geometry/Frustum.h"

class m1Camera3D :
	public Module
{
	friend class p1Configuration;
public:
	m1Camera3D(bool start_enabled = true);
	~m1Camera3D();

	bool Start() override;

	UpdateStatus PreUpdate() override;
	UpdateStatus Update() override;

	void CameraMovement();

private:
	Frustum frustum;

	float2 lastRight;
	float2 lastMiddle;

	float pan_speed = 0.5f;
	float orbit_speed = 0.1f;
	float zoom_speed = 15.f;
};

