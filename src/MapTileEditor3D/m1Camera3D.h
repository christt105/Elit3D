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

	void SetFov(float vertical_angle);
	void SetFov();

	UpdateStatus PreUpdate() override;
	UpdateStatus Update() override;

	void CameraMovement();

	Frustum frustum;

private:
	float2 lastRight;
	float2 lastMiddle;

	float FOV = 60.f;

	float pan_speed = 0.5f;
	float orbit_speed = 0.1f;
	float zoom_speed = 15.f;
	float mov_speed = 15.f;
	float turbo_speed = 2.f;
};

