#include "m1Camera3D.h"

#include "ExternalTools/MathGeoLib/include/Math/MathFunc.h"
#include "ExternalTools/MathGeoLib/include/Math/Quat.h"

#include "Application.h"
#include "m1Input.h"
#include "m1Window.h"
#include "m1Render3D.h"
#include "Shader.h"

#include "Logger.h"

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
	frustum.pos = float3(0.f, 0.f, -5.f);
	frustum.front = float3::unitZ;
	frustum.up = float3::unitY;
	
	frustum.nearPlaneDistance = 1.f;
	frustum.farPlaneDistance = 100.f;

	frustum.verticalFov = DegToRad(60.f);
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * App->window->GetWidth() / App->window->GetHeight());

	return ret;
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
	float speed = 15.f;
	if (App->input->IsKeyPressed(SDL_SCANCODE_W))
		frustum.pos += frustum.front.Normalized() * speed * App->GetDt();
	if (App->input->IsKeyPressed(SDL_SCANCODE_S))
		frustum.pos -= frustum.front.Normalized() * speed * App->GetDt();
	if (App->input->IsKeyPressed(SDL_SCANCODE_A))
		frustum.pos -= frustum.front.Cross(frustum.up) * speed * App->GetDt();
	if (App->input->IsKeyPressed(SDL_SCANCODE_D))
		frustum.pos += frustum.front.Cross(frustum.up) * speed * App->GetDt();

	if (App->input->IsKeyPressed(SDL_SCANCODE_R))
		frustum.pos += float3::unitY * speed * App->GetDt();
	if (App->input->IsKeyPressed(SDL_SCANCODE_F))
		frustum.pos -= float3::unitY * speed * App->GetDt();

	if (App->input->IsMouseButtonPressed(SDL_BUTTON_RIGHT)) {
		if (App->input->IsMouseButtonDown(SDL_BUTTON_RIGHT)) {
			lastX = App->input->GetMouseX();
			lastY = App->input->GetMouseY();
		}

		currentX = App->input->GetMouseX();
		currentY = App->input->GetMouseY();

		int xoffset = currentX - lastX;
		int yoffset = currentY - lastY;

		lastX = currentX;
		lastY = currentY;

		float smooth = 0.1f;

		Quat rot = Quat(float3::unitY, -xoffset * smooth * App->GetDt());
		Quat rot2 = Quat(frustum.up.Cross(frustum.front), yoffset * smooth * App->GetDt());

		frustum.front = rot * rot2 * frustum.front;
		float3 right = frustum.front.Cross(float3::unitY);
		frustum.up = rot * rot2 * frustum.up;
	}
}
