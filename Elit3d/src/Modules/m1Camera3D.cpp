#include "Modules/m1Camera3D.h"

#include "ExternalTools/MathGeoLib/include/MathGeoLib.h"

#include "Tools/Camera.h"

#include "Core/Application.h"
#include "Modules/m1Input.h"
#include "Modules/m1Window.h"
#include "Modules/m1Render3D.h"
#include "Modules/m1GUI.h"

#include "Modules/m1MapEditor.h"
#include "Resources/r1Map.h"

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
	scene = CreateCamera("scene");

	scene->frustum.SetVerticalFovAndAspectRatio(DegToRad(60.f), (float)App->window->GetWidth() / (float)App->window->GetHeight());

	scene->mov_speed = node.value("mov_speed", 15.0f);
	scene->orbit_speed = node.value("orbit_speed", 0.1f);
	scene->pan_speed = node.value("pan_speed", 0.5f);
	scene->turbo_speed = node.value("turbo_speed", 2.f);
	scene->zoom_speed = node.value("zoom_speed", 15.0f);

	return true;
}

void m1Camera3D::SetFov()
{
	//frustum.SetVerticalFovAndAspectRatio(FOV, (float)App->window->GetWidth() / (float)App->window->GetHeight());
}

UpdateStatus m1Camera3D::PreUpdate()
{
	PROFILE_FUNCTION();

	for (auto c : cameras) {
		if (c->is_active)
			c->CameraMovement();
	}
	
	return UpdateStatus::UPDATE_CONTINUE;
}

bool m1Camera3D::CleanUp()
{
	for (auto c : cameras)
		delete c;
	
	return true;
}

Camera* m1Camera3D::CreateCamera(const char* id)
{
	for (auto c : cameras)
		if (strcmp(c->id, id) == 0)
			return c;
	Camera* ret = new Camera(id);
	cameras.push_back(ret);
	return ret;
}

void m1Camera3D::UpdateShaders(Camera* cam)
{
	auto shader = App->render->GetShader("default");
	shader->Use();
	shader->SetMat4("model", float4x4::identity);
	shader->SetMat4("view", cam->frustum.ViewMatrix());
	shader->SetMat4("projection", cam->frustum.ProjectionMatrix());

	shader->SetVec3("lightPos", cam->frustum.Pos());

	shader = App->render->GetShader("tilemap");
	shader->Use();
	shader->SetMat4("model", float4x4::identity);
	shader->SetMat4("view", cam->frustum.ViewMatrix());
	shader->SetMat4("projection", cam->frustum.ProjectionMatrix());

	shader = App->render->GetShader("selectTile");
	shader->Use();
	shader->SetMat4("view", cam->frustum.ViewMatrix());
	shader->SetMat4("projection", cam->frustum.ProjectionMatrix());

	shader = App->render->GetShader("grid");
	shader->Use();
	shader->SetMat4("view", cam->frustum.ViewMatrix());
	shader->SetMat4("projection", cam->frustum.ProjectionMatrix());
	
	shader = App->render->GetShader("selectTile");
	shader->Use();
	shader->SetMat4("view", cam->frustum.ViewMatrix());
	shader->SetMat4("projection", cam->frustum.ProjectionMatrix());
}

void m1Camera3D::FitOnMap() const
{
	r1Map const* map = App->map_editor->GetMap();
	if (map == nullptr)
		return;
	int2 size = map->GetSize();
	scene->frustum.SetPos(float3((float)size.x * 0.5f, 20.f, -10.f)); //TODO: Improve the fit
	scene->LookAt(float3((float)size.x * 0.5f, 0.f, (float)size.y * 0.5f));
}

void m1Camera3D::TopView() const
{
	r1Map const* map = App->map_editor->GetMap();
	if (map == nullptr)
		return;
	int2 size = map->GetSize();
	scene->frustum.SetPos(float3((float)size.x * 0.5f, 20.f, (float)size.y * 0.5f)); //TODO: Improve the fit
	scene->LookAt(float3((float)size.x * 0.5f, 0.f, (float)size.y * 0.5f));
}

void m1Camera3D::FrontView() const
{
	r1Map const* map = App->map_editor->GetMap();
	if (map == nullptr)
		return;
	int2 size = map->GetSize();
	scene->frustum.SetPos(float3((float)size.x * 0.5f, 5.f, -10.f)); //TODO: Improve the fit
	scene->frustum.SetFront(float3::unitZ);
	scene->frustum.SetUp(float3::unitY);
}

void m1Camera3D::Save(nlohmann::json& node)
{
	/*node["FOV"] = FOV;
	node["pan_speed"] = pan_speed;
	node["orbit_speed"] = orbit_speed;
	node["zoom_speed"] = zoom_speed;
	node["mov_speed"] = mov_speed;
	node["turbo_speed"] = turbo_speed;*/
}

void m1Camera3D::Load(const nlohmann::json& node)
{
	/*FOV = node["FOV"];
	SetFov(FOV);
	pan_speed = node["pan_speed"];
	orbit_speed = node["orbit_speed"];
	zoom_speed = node["zoom_speed"];
	mov_speed = node["mov_speed"];
	turbo_speed = node["turbo_speed"];*/
}
