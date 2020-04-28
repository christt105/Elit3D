#include "Application.h"

#include "Logger.h"

#include "m1Input.h"
#include "m1Window.h"
#include "m1Render3D.h"
#include "m1Scene.h"
#include "m1GUI.h"
#include "ModuleObjects.h"

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
	objects = new ModuleObjects();

	modules.push_back(input);
	modules.push_back(window);

	modules.push_back(scene);
	modules.push_back(objects);

	modules.push_back(gui);
	modules.push_back(render);

	file_system = new FileSystem();

	nlohmann::json conf = file_system->OpenJSONFile("Configuration/Configuration.json")["App"];

	if (conf.is_null())
		LOGE("Configuration.json not found");

	name.assign(conf.value("name", "PROGRAM"));

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

UpdateStatus Application::Update()
{
	UpdateStatus ret = UpdateStatus::UPDATE_CONTINUE;

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

	return ret;
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
