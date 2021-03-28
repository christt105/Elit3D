#pragma once

#include "ExternalTools/MathGeoLib/include/Geometry/Frustum.h"

class Camera {
	friend class m1Camera3D;
public:
	enum class RotationType {
		None,
		FirstPerson,
		Orbit
	};
private:
	Camera(const char* id);
	~Camera();

public:
	void CameraMovement();
	void UpdateFrustum(int sizeX, int sizeY);
	void ImGuiControl();
	void LookAt(const float3& position);

	inline const Frustum& GetFrustum() const { return frustum; }

public:
	bool is_active = true;

	bool wasd_mov = true;
	bool rf_mov = true;
	RotationType rotation = RotationType::FirstPerson;
	float3 pivot = float3::zero;
	bool pan_mov = true;
	bool zoom_mov = true;

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