#include "m1Camera3D.h"

#include "ExternalTools/MathGeoLib/include/MathGeoLib.h"

#include "Application.h"
#include "m1Input.h"
#include "m1Window.h"
#include "m1Render3D.h"
#include "m1GUI.h"

#include "p1Scene.h"

#include "r1Shader.h"

#include "Logger.h"

#include "Profiler.h"

#include "ExternalTools/mmgr/mmgr.h"

m1Camera3D::m1Camera3D(bool start_enabled) : Module("Camera3D", start_enabled)
{
}

m1Camera3D::~m1Camera3D()
{
}

bool m1Camera3D::Start()
{
	PROFILE_FUNCTION();
	bool ret = true;

	frustum.type = FrustumType::PerspectiveFrustum;
	frustum.pos = { 0, 15, 0 };
	frustum.front = -float3::unitY;
	frustum.up = float3::unitZ;

	SetFov(60.f);
	
	frustum.nearPlaneDistance = 0.1f;
	frustum.farPlaneDistance = 1000.f;

	return ret;
}

void m1Camera3D::SetFov(float vertical_angle)
{
	FOV = vertical_angle;
	SetFov();
}

void m1Camera3D::SetFov()
{
	frustum.verticalFov = DegToRad(60.f);
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * App->window->GetWidth() / App->window->GetHeight());
}

UpdateStatus m1Camera3D::PreUpdate()
{
	PROFILE_FUNCTION();
	CameraMovement();

	static auto m = float4x4::FromTRS(float3::zero, Quat::identity, float3(-1.f, 1.f, 1.f)); // TODO: research about inverted x

	auto shader = App->render->GetShader("default");
	shader->Use();
	shader->SetMat4("model", float4x4::identity);
	shader->SetMat4("view", frustum.ViewMatrix());
	shader->SetMat4("projection", frustum.ProjectionMatrix());

	shader = App->render->GetShader("tilemap");
	shader->Use();
	shader->SetMat4("model", float4x4::identity);
	shader->SetMat4("view", frustum.ViewMatrix() * m);
	shader->SetMat4("projection", frustum.ProjectionMatrix());

	shader = App->render->GetShader("grid");
	shader->Use();
	shader->SetMat4("view", frustum.ViewMatrix());
	shader->SetMat4("projection", frustum.ProjectionMatrix());
	
	return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus m1Camera3D::Update()
{
	return UpdateStatus::UPDATE_CONTINUE;
}

void m1Camera3D::CameraMovement()
{
	if (App->gui->scene->IsFocused() && App->gui->scene->IsOnHover()) {
		float speed = mov_speed;
		if (App->input->IsKeyPressed(SDL_SCANCODE_LSHIFT))
			speed *= turbo_speed;
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
	}

	if (App->gui->scene->IsOnHover()) {
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
			ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeAll);
			if (App->input->IsMouseButtonDown(SDL_BUTTON_MIDDLE)) {
				lastMiddle = float2((float)App->input->GetMouseX(), (float)App->input->GetMouseY());
			}
			float2 current = float2((float)App->input->GetMouseX(), (float)App->input->GetMouseY());
			float2 offset = (current - lastMiddle) * pan_speed;

			lastMiddle = current;

			frustum.pos += frustum.up.Cross(frustum.front) * offset.x * App->GetDt() + frustum.up * offset.y * App->GetDt();
		}
		if (App->input->GetMouseZ() != 0) {
			switch (frustum.type)
			{
			case FrustumType::OrthographicFrustum: {
				/*float val = (float)App->input->GetMouseZ() * zoom_speed * App->GetDt();
				static float size = 5.f;
				size += val;
				frustum.orthographicHeight = 2.f * val;
				frustum.orthographicWidth = 2.f* frustum.orthographicHeight * frustum.AspectRatio();*/
				break;
			}
			case FrustumType::PerspectiveFrustum:
				frustum.pos += frustum.front * (float)App->input->GetMouseZ() * zoom_speed * App->GetDt();
				break;
			default:
				break;
			}
		}
	}
}
