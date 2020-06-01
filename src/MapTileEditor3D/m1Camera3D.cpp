#include "m1Camera3D.h"

#include "ExternalTools/MathGeoLib/include/Math/MathFunc.h"
#include "ExternalTools/MathGeoLib/include/Math/Quat.h"

#include "Application.h"
#include "m1Input.h"
#include "m1Window.h"
#include "m1Render3D.h"
#include "r1Shader.h"

#include "Logger.h"

#include "ExternalTools/mmgr/mmgr.h"

m1Camera3D::m1Camera3D(bool start_enabled) : Module("Camera3D", start_enabled)
{
}

m1Camera3D::~m1Camera3D()
{
}

bool m1Camera3D::Start()
{
	bool ret = true;

	frustum.type = FrustumType::PerspectiveFrustum;
	frustum.pos = float3(0.f, 1.f, -5.f);
	frustum.front = float3::unitZ;
	frustum.up = float3::unitY;
	
	frustum.nearPlaneDistance = 0.1f;
	frustum.farPlaneDistance = 1000.f;

	SetFov(60.f);

	return ret;
}

void m1Camera3D::SetFov(float vertical_angle)
{
	FOV = vertical_angle;
	SetFov();
}

void m1Camera3D::SetFov()
{
	frustum.verticalFov = DegToRad(FOV);
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * App->window->GetWidth() / App->window->GetHeight());
}

UpdateStatus m1Camera3D::PreUpdate()
{
	CameraMovement();
	App->render->bShader->SetMat4("model", float4x4::FromTRS(float3::zero, Quat::identity, float3::one));
	App->render->bShader->SetMat4("view", frustum.ViewMatrix());
	App->render->bShader->SetMat4("projection", frustum.ProjectionMatrix());
	
	return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus m1Camera3D::Update()
{
	return UpdateStatus::UPDATE_CONTINUE;
}

void m1Camera3D::CameraMovement()
{
	if (App->input->IsKeyPressed(SDL_SCANCODE_W))
		frustum.pos += frustum.front.Normalized() * mov_speed * App->GetDt();
	if (App->input->IsKeyPressed(SDL_SCANCODE_S))
		frustum.pos -= frustum.front.Normalized() * mov_speed * App->GetDt();
	if (App->input->IsKeyPressed(SDL_SCANCODE_A))
		frustum.pos -= frustum.front.Cross(frustum.up) * mov_speed * App->GetDt();
	if (App->input->IsKeyPressed(SDL_SCANCODE_D))
		frustum.pos += frustum.front.Cross(frustum.up) * mov_speed * App->GetDt();

	if (App->input->IsKeyPressed(SDL_SCANCODE_R))
		frustum.pos += float3::unitY * mov_speed * App->GetDt();
	if (App->input->IsKeyPressed(SDL_SCANCODE_F))
		frustum.pos -= float3::unitY * mov_speed * App->GetDt();

	if (App->input->IsMouseButtonPressed(SDL_BUTTON_RIGHT)) {
		if (App->input->IsMouseButtonDown(SDL_BUTTON_RIGHT)) {
			lastRight = float2((float)App->input->GetMouseX(), (float)App->input->GetMouseY());
		}
		float2 current = float2((float)App->input->GetMouseX(), (float)App->input->GetMouseY());
		float2 offset = (current - lastRight) * orbit_speed;

		lastRight = current;

		Quat rot = Quat(float3::unitY, -offset.x * App->GetDt()) * Quat(frustum.up.Cross(frustum.front), offset.y * App->GetDt());

		float3 up = rot * frustum.up;
		if (up.y >= 0) { // Block rotation camera will be upside down
			frustum.front = rot * frustum.front;
			frustum.up = up;
		}
	}
	if (App->input->IsMouseButtonPressed(SDL_BUTTON_MIDDLE)) {
		if (App->input->IsMouseButtonDown(SDL_BUTTON_MIDDLE)) {
			lastMiddle = float2((float)App->input->GetMouseX(), (float)App->input->GetMouseY());
		}
		float2 current = float2((float)App->input->GetMouseX(), (float)App->input->GetMouseY());
		float2 offset = (current - lastMiddle) * pan_speed;

		lastMiddle = current;

		frustum.pos += frustum.up.Cross(frustum.front) * offset.x * App->GetDt() + frustum.up * offset.y * App->GetDt();
	}
	if (App->input->GetMouseZ() != 0) {
		frustum.pos += frustum.front * (float)App->input->GetMouseZ() * zoom_speed * App->GetDt();
	}
}
