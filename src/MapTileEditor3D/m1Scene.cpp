#include "m1Scene.h"

#include <GL/glew.h>
#include <SDL.h>

#include "Application.h"
#include "m1Input.h"

#include "Logger.h"

#include "ExternalTools/mmgr/mmgr.h"

m1Scene::m1Scene(bool start_enabled) : Module("Scene", start_enabled)
{
}

m1Scene::~m1Scene()
{
}

bool m1Scene::Init(const nlohmann::json& node)
{
	return true;
}

bool m1Scene::Start()
{	
	return true;
}

UpdateStatus m1Scene::Update()
{
	if (App->input->IsKeyDown(SDL_SCANCODE_ESCAPE))
		return UpdateStatus::UPDATE_STOP;

	return UpdateStatus::UPDATE_CONTINUE;
}

bool m1Scene::CleanUp()
{
	return true;
}
