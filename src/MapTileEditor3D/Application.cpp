#include "Application.h"

#include <Windows.h>
#include <shellapi.h>

#include "Logger.h"

#include "m1Input.h"
#include "m1Window.h"
#include "m1Render3D.h"
#include "m1Scene.h"
#include "m1GUI.h"
#include "m1Objects.h"
#include "m1Camera3D.h"

#include "FileSystem.h"

#include "ExternalTools/mmgr/mmgr.h"

Application::Application() {

}

Application::~Application()	{

}

bool Application::Init()
{
	input = new m1Input();
	window = new m1Window();
	render = new m1Render3D();
	scene = new m1Scene();
	gui = new m1GUI();
	objects = new m1Objects();
	camera = new m1Camera3D();

	modules.push_back(input);
	modules.push_back(window);

	modules.push_back(camera);
	modules.push_back(scene);
	modules.push_back(objects);

	modules.push_back(gui);
	modules.push_back(render);

	file_system = new FileSystem();

	nlohmann::json conf = file_system->OpenJSONFile("Configuration/Configuration.json")["App"];

	if (conf.is_null())
		LOGE("Configuration.json not found");

	name.assign(conf.value("name", "PROGRAM"));
	version.assign(conf.value("version", "0.1"));

	for (auto i = modules.begin(); i != modules.end(); ++i) {
		LOG("Initializing module %s", (*i)->name.c_str());
		(*i)->Init(conf[(*i)->name]);
	}
	

	return true;
}

bool Application::Start()
{
	for (auto i = modules.begin(); i != modules.end(); ++i) {
		LOG("Starting module %s", (*i)->name.c_str());
		(*i)->Start();
	}
	return true;
}

void Application::PrepareUpdate()
{
	last_time = time;
	time = SDL_GetPerformanceCounter();
	++frame_count;

	dt = (float)((time - last_time) / (double)SDL_GetPerformanceFrequency());
}

UpdateStatus Application::Update()
{
	UpdateStatus ret = UpdateStatus::UPDATE_CONTINUE;

	PrepareUpdate();
	
	for (auto i = modules.begin(); i != modules.end(); ++i) {
		ret = (*i)->PreUpdate();
		if (ret != UpdateStatus::UPDATE_CONTINUE) {
			LOG("Module %s returned %s on PreUpdate()", (*i)->name.c_str(), Module::UpdateStatusToString(ret).c_str());
			return ret;
		}
	}
	
	for (auto i = modules.begin(); i != modules.end(); ++i) {
		ret = (*i)->Update();
		if (ret != UpdateStatus::UPDATE_CONTINUE) {
			LOG("Module %s returned %s on PreUpdate()", (*i)->name.c_str(), Module::UpdateStatusToString(ret).c_str());
			return ret;
		}
	}
	
	for (auto i = modules.begin(); i != modules.end(); ++i) {
		ret = (*i)->PostUpdate();
		if (ret != UpdateStatus::UPDATE_CONTINUE) {
			LOG("Module %s returned %s on PreUpdate()", (*i)->name.c_str(), Module::UpdateStatusToString(ret).c_str());
			return ret;
		}
	}

	FinishUpdate();

	return ret;
}

void Application::FinishUpdate()
{
}

bool Application::CleanUp()
{
	bool ret = true;

	delete file_system;

	for (auto i = modules.rbegin(); i != modules.rend(); ++i) {
		LOG("Cleaning Up module %s", (*i)->name.c_str());
		ret = (*i)->CleanUp();
		delete* i;
	}

	return ret;
}

const char* Application::GetName()
{
	return name.c_str();
}

inline const char* Application::GetName() const
{
	return name.c_str();
}

const char* Application::GetVersion()
{
	return version.c_str();
}

const char* Application::GetVersion() const
{
	return version.c_str();
}

float Application::GetDt() const
{
	return dt;
}

unsigned int Application::GetFrames() const
{
	return frame_count;
}

void Application::ExecuteURL(const char* url)
{
	// TODO: set it for linux and mac
	ShellExecute(0, 0, url, 0, 0, SW_SHOW);
}
