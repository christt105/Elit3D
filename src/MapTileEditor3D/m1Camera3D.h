#pragma once
#include "Module.h"

#include "ExternalTools/MathGeoLib/include/Geometry/Frustum.h"

class m1Camera3D :
	public Module
{
public:
	m1Camera3D(bool start_enabled = true);
	~m1Camera3D();

	bool Start() override;

	UpdateStatus PreUpdate() override;
	UpdateStatus Update() override;

	void CameraMovement();

private:
	Frustum frustum;

	int currentX = 0;
	int currentY = 0;
	int lastX = 0;
	int lastY = 0;
};

