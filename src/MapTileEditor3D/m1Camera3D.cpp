#include "m1Camera3D.h"

#include "ExternalTools/MathGeoLib/include/Math/MathFunc.h"
#include "ExternalTools/MathGeoLib/include/Math/Quat.h"

#include "Application.h"
#include "m1Input.h"
#include "m1Window.h"
#include "m1Render3D.h"
#include "Shader.h"

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

	frustum.verticalFov = 2.f * atanf(App->window->GetAspectRatio());
	frustum.horizontalFov = App->window->GetAspectRatio() * frustum.verticalFov;

	return ret;
}

UpdateStatus m1Camera3D::PreUpdate()
{
	App->render->bShader->SetMat4("model", float4x4::FromTRS(float3::zero, Quat::identity, float3::one));
	App->render->bShader->SetMat4("view", frustum.ViewMatrix());
	App->render->bShader->SetMat4("projection", frustum.ProjectionMatrix());
	
	return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus m1Camera3D::Update()
{
	float speed = 15.f;
	if (App->input->IsKeyPressed(SDL_SCANCODE_W))
		frustum.pos += frustum.front.Normalized() * speed * App->GetDt();
	if (App->input->IsKeyPressed(SDL_SCANCODE_S))
		frustum.pos += -frustum.front.Normalized() * speed * App->GetDt();

	if (App->input->IsKeyPressed(SDL_SCANCODE_A))
		frustum.front = Quat::RotateY(0.1f * speed * App->GetDt()) * frustum.front;
	if (App->input->IsKeyPressed(SDL_SCANCODE_D))
		frustum.front = Quat::RotateY(-0.1f * speed * App->GetDt()) * frustum.front;

	if (App->input->IsKeyPressed(SDL_SCANCODE_R))
		frustum.pos += float3(0.f, 1.f, 0.f) * speed * App->GetDt();
	if (App->input->IsKeyPressed(SDL_SCANCODE_F))
		frustum.pos += float3(0.f, -1.f, 0.f) * speed * App->GetDt();

	return UpdateStatus::UPDATE_CONTINUE;
}
