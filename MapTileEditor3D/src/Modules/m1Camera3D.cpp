#include "Modules/m1Camera3D.h"

#include "ExternalTools/MathGeoLib/include/MathGeoLib.h"

#include "Core/Application.h"
#include "Modules/m1Input.h"
#include "Modules/m1Window.h"
#include "Modules/m1Render3D.h"
#include "Modules/m1GUI.h"

#include "Panels/p1Scene.h"

#include "Resources/r1Shader.h"

#include "Tools/System/Logger.h"

#include "Tools/System/Profiler.h"

#include "ExternalTools/mmgr/mmgr.h"

m1Camera3D::m1Camera3D(bool start_enabled) : Module("Camera3D", start_enabled)
{
}

m1Camera3D::~m1Camera3D()
{
}

bool m1Camera3D::Init(const nlohmann::json& node)
{
	frustum.SetFrame({ 0, 15, 0 }, -float3::unitY, float3::unitZ);
	frustum.SetKind(FrustumProjectiveSpace::FrustumSpaceGL, FrustumHandedness::FrustumLeftHanded);

	//SetFov(node.value("FOV", 60.f));
	SetFov(math::DegToRad(60.f));

	auto t = frustum.Type();

	mov_speed = node.value("mov_speed", 15.0f);
	orbit_speed = node.value("orbit_speed", 0.1f);
	pan_speed = node.value("pan_speed", 0.5f);
	turbo_speed = node.value("turbo_speed", 2.f);
	zoom_speed = node.value("zoom_speed", 15.0f);

	return false;
}

bool m1Camera3D::Start()
{
	PROFILE_FUNCTION();
	bool ret = true;
	
	frustum.SetViewPlaneDistances(0.1f, 1000.f);

	return ret;
}

void m1Camera3D::SetFov(float vertical_angle)
{
	FOV = vertical_angle;
	SetFov();
}

void m1Camera3D::SetFov()
{
	frustum.SetVerticalFovAndAspectRatio(FOV, (float)App->window->GetWidth() / (float)App->window->GetHeight());
}

UpdateStatus m1Camera3D::PreUpdate()
{
	PROFILE_FUNCTION();
	CameraMovement();

	auto shader = App->render->GetShader("default");
	shader->Use();
	shader->SetMat4("model", float4x4::identity);
	shader->SetMat4("view", frustum.ViewMatrix());
	shader->SetMat4("projection", frustum.ProjectionMatrix());

	shader = App->render->GetShader("tilemap");
	shader->Use();
	shader->SetMat4("model", float4x4::identity);
	shader->SetMat4("view", frustum.ViewMatrix());
	shader->SetMat4("projection", frustum.ProjectionMatrix());

	shader = App->render->GetShader("grid");
	shader->Use();
	shader->SetMat4("view", frustum.ViewMatrix());
	shader->SetMat4("projection", frustum.ProjectionMatrix());

	shader = App->render->GetShader("selectTile");
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
			frustum.Translate(frustum.Front().Normalized() * speed * App->GetDt());
		if (App->input->IsKeyPressed(SDL_SCANCODE_S))
			frustum.Translate(-frustum.Front().Normalized() * speed * App->GetDt());
		if (App->input->IsKeyPressed(SDL_SCANCODE_A))
			frustum.Translate(frustum.Front().Cross(frustum.Up()) * speed * App->GetDt());
		if (App->input->IsKeyPressed(SDL_SCANCODE_D))
			frustum.Translate(-frustum.Front().Cross(frustum.Up()) * speed * App->GetDt());

		if (App->input->IsKeyPressed(SDL_SCANCODE_R))
			frustum.Translate(float3::unitY * speed * App->GetDt());
		if (App->input->IsKeyPressed(SDL_SCANCODE_F))
			frustum.Translate(-float3::unitY * speed * App->GetDt());
	}

	if (App->gui->scene->IsOnHover()) {
		if (App->input->IsMouseButtonPressed(SDL_BUTTON_RIGHT)) {
			if (App->input->IsMouseButtonDown(SDL_BUTTON_RIGHT) || lastRight.x == -1.f || lastRight.y == -1.f) {
				lastRight = float2((float)App->input->GetMouseX(), (float)App->input->GetMouseY());
			}
			float2 current = float2((float)App->input->GetMouseX(), (float)App->input->GetMouseY());
			float2 offset = (current - lastRight) * orbit_speed;

			lastRight = current;

			Quat rot = Quat(float3::unitY, offset.x * App->GetDt()) * Quat(frustum.Up().Cross(frustum.Front()), offset.y * App->GetDt());

			float3 up = rot * frustum.Up();
			if (up.y >= 0) { // Block rotation camera will be upside down
				frustum.SetFront(rot * frustum.Front());
				frustum.SetUp(up);
			}
		}
		if (App->input->IsMouseButtonPressed(SDL_BUTTON_MIDDLE)) {
			ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeAll);
			if (App->input->IsMouseButtonDown(SDL_BUTTON_MIDDLE) || lastMiddle.x == -1.f || lastMiddle.y == -1.f) {
				lastMiddle = float2((float)App->input->GetMouseX(), (float)App->input->GetMouseY());
			}
			float2 current = float2((float)App->input->GetMouseX(), (float)App->input->GetMouseY());
			float2 offset = (lastMiddle - current) * pan_speed;

			lastMiddle = current;

			frustum.Translate(frustum.Up().Cross(frustum.Front()) * offset.x * App->GetDt() + frustum.Up() * -offset.y * App->GetDt());
		}
		if (App->input->GetMouseZ() != 0) {
			switch (frustum.Type())
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
				frustum.Translate(frustum.Front() * (float)App->input->GetMouseZ() * zoom_speed * App->GetDt());
				break;
			default:
				break;
			}
		}
	}
}

void m1Camera3D::Save(nlohmann::json& node)
{
	node["FOV"] = FOV;
	node["pan_speed"] = pan_speed;
	node["orbit_speed"] = orbit_speed;
	node["zoom_speed"] = zoom_speed;
	node["mov_speed"] = mov_speed;
	node["turbo_speed"] = turbo_speed;
}

void m1Camera3D::Load(const nlohmann::json& node)
{
	FOV = node["FOV"];
	SetFov(FOV);
	pan_speed = node["pan_speed"];
	orbit_speed = node["orbit_speed"];
	zoom_speed = node["zoom_speed"];
	mov_speed = node["mov_speed"];
	turbo_speed = node["turbo_speed"];
}
