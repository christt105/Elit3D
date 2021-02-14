#pragma once

#include "ExternalTools/MathGeoLib/include/Geometry/Frustum.h"

class Camera {
	friend class m1Camera3D;
private:
	Camera(const char* id);
	~Camera();

public:
	void CameraMovement();
	void UpdateFrustum(int sizeX, int sizeY);
	void ImGuiControl();

public:
	bool is_active = true;

private:
	Frustum frustum;

	const char* id = "";


	float2 lastRight = { -1.f, -1.f };
	float2 lastMiddle = { -1.f, -1.f };

	float FOV = 60.f;

	float pan_speed = 0.5f;
	float orbit_speed = 0.1f;
	float zoom_speed = 15.f;
	float mov_speed = 15.f;
	float turbo_speed = 2.f;
};