#include "Camera.h"

#include "Core/Application.h"
#include "Modules/m1Input.h"

#include "ExternalTools/MathGeoLib/include/Math/Quat.h"

#include "ExternalTools/ImGui/imgui.h"
#include "Tools/System/Logger.h"

Camera::Camera(const char* id) : id(id)
{
	frustum.SetFrame({ 2.f, 7.f, -10.f }, float3::unitZ, float3::unitY);
	frustum.SetKind(FrustumProjectiveSpace::FrustumSpaceGL, FrustumHandedness::FrustumLeftHanded);
	FOV = DegToRad(60.f);
	frustum.SetViewPlaneDistances(0.1f, 1000.f);
}

Camera::~Camera()
{
}

void Camera::CameraMovement()
{
	{
		float speed = mov_speed;
		if (wasd_mov) {
			if (App->input->IsKeyPressed(SDL_SCANCODE_LSHIFT))
				speed *= turbo_speed;
			if (App->input->IsKeyPressed(SDL_SCANCODE_W))
				Zoom(speed * App->GetDt());
			if (App->input->IsKeyPressed(SDL_SCANCODE_S))
				Zoom(-speed * App->GetDt());
			if (App->input->IsKeyPressed(SDL_SCANCODE_A))
				frustum.Translate(frustum.Front().Cross(frustum.Up()) * speed * App->GetDt());
			if (App->input->IsKeyPressed(SDL_SCANCODE_D))
				frustum.Translate(-frustum.Front().Cross(frustum.Up()) * speed * App->GetDt());
		}

		if (rf_mov) {
			if (App->input->IsKeyPressed(SDL_SCANCODE_R))
				frustum.Translate(float3::unitY * speed * App->GetDt());
			if (App->input->IsKeyPressed(SDL_SCANCODE_F))
				frustum.Translate(-float3::unitY * speed * App->GetDt());
		}
	}

	if (rotation != RotationType::None && App->input->IsMouseButtonPressed(SDL_BUTTON_RIGHT)) {
		if (App->input->IsMouseButtonDown(SDL_BUTTON_RIGHT) || lastRight.x == -1.f || lastRight.y == -1.f) {
			lastRight = float2((float)App->input->GetMouseX(), (float)App->input->GetMouseY());
		}
		float2 current = float2((float)App->input->GetMouseX(), (float)App->input->GetMouseY());
		float2 offset = (current - lastRight) * orbit_speed;

		lastRight = current;

		if (rotation == RotationType::FirstPerson) {

			Quat rot = Quat(float3::unitY, offset.x * App->GetDt()) * Quat(frustum.Up().Cross(frustum.Front()), offset.y * App->GetDt());

			float3 up = rot * frustum.Up();
			if (up.y >= 0) { // Block rotation camera will be upside down
				frustum.SetFront(rot * frustum.Front());
				frustum.SetUp(up);
			}
		}
		else {
			Quat rotx = Quat::RotateAxisAngle(float3::unitY, offset.x * App->GetDt());
			Quat roty = Quat::RotateAxisAngle(frustum.WorldRight(), offset.y * App->GetDt());

			float3 up = rotx * roty * frustum.Up();
			frustum.SetFront(rotx * roty * frustum.Front());
			frustum.SetUp(up);

			frustum.SetPos(rotx * roty * (frustum.Pos() - pivot) + pivot);

			LookAt(pivot);
		}
	}

	if (pan_mov && App->input->IsMouseButtonPressed(SDL_BUTTON_MIDDLE)) {
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
		if (App->input->IsMouseButtonDown(SDL_BUTTON_MIDDLE) || lastMiddle.x == -1.f || lastMiddle.y == -1.f) {
			lastMiddle = float2((float)App->input->GetMouseX(), (float)App->input->GetMouseY());
		}
		float2 current = float2((float)App->input->GetMouseX(), (float)App->input->GetMouseY());
		float2 offset = (lastMiddle - current) * pan_speed;

		lastMiddle = current;

		frustum.Translate(frustum.Up().Cross(frustum.Front()) * offset.x * App->GetDt() + frustum.Up() * -offset.y * App->GetDt());
	}

	if (zoom_mov && App->input->GetMouseZ() != 0) {
		Zoom((float)App->input->GetMouseZ() * zoom_speed * App->GetDt());
	}
}

void Camera::UpdateFrustum(int sizeX, int sizeY)
{
	if (frustum.Type() == FrustumType::PerspectiveFrustum) {
		frustum.SetVerticalFovAndAspectRatio(FOV, (float)sizeX / (float)sizeY);
	}
	else {
		float aspect = (float)sizeX / (float)sizeY;
		frustum.SetOrthographic(frustum.OrthographicWidth(), frustum.OrthographicWidth() / aspect);
	}
}

void Camera::Zoom(float value) {
	if (frustum.Type() == FrustumType::PerspectiveFrustum) {
		frustum.Translate(frustum.Front() * value);
	}
	else {
		value *= -2.f;
		float aspect = frustum.OrthographicWidth() / frustum.OrthographicHeight();
		if (frustum.OrthographicWidth() + value > 0.f)
			frustum.SetOrthographic(frustum.OrthographicWidth() + value, (frustum.OrthographicWidth() + value) / aspect);
	}
}

void Camera::ImGuiControl()
{
#define ORANGE ImVec4(0.f,1.f,0.f,1.f)
	ImGui::Text("Camera Position: "); ImGui::SameLine(); ImGui::TextColored(ORANGE, "%s", frustum.Pos().ToString().c_str());
	ImGui::Text("Camera Front Axis: "); ImGui::SameLine(); ImGui::TextColored(ORANGE, "%s", frustum.Front().ToString().c_str());
	ImGui::Text("Camera Up Axis: "); ImGui::SameLine(); ImGui::TextColored(ORANGE, "%s", frustum.Up().ToString().c_str());
	ImGui::SliderFloat("Pan Speed", &pan_speed, 0.1f, 2.f);
	ImGui::SliderFloat("Orbit Speed", &orbit_speed, 0.01f, 0.5f);
	ImGui::SliderFloat("Zoom Speed", &zoom_speed, 1.f, 100.f);
	ImGui::SliderFloat("Movement Speed", &mov_speed, 1.f, 50.f);
	ImGui::SliderFloat("Turbo Speed", &turbo_speed, 1.f, 50.f);
	if (float fov_deg = RadToDeg(FOV); ImGui::SliderFloat("FOV", &fov_deg, 30.f, 120.f)) {
		FOV = DegToRad(fov_deg);
	}
}

void Camera::LookAt(const float3& position)
{
	Quat look = Quat::LookAt(frustum.Front(), (position - frustum.Pos()).Normalized(), frustum.Up(), float3::unitY);
	frustum.SetFront(look * frustum.Front());
	frustum.SetUp(look * frustum.Up());
}
