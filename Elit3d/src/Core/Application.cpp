#include "Core/Application.h"

#include <Windows.h>

#include "Tools/System/Logger.h"

#include "Modules/m1Input.h"
#include "Modules/m1Window.h"
#include "Modules/m1Render3D.h"
#include "Modules/m1Scene.h"
#include "Modules/m1GUI.h"
#include "Modules/m1Objects.h"
#include "Modules/m1Camera3D.h"
#include "Modules/m1Resources.h"
#include "Modules/m1Importer.h"
#include "Modules/m1Events.h"
#include "Modules/m1MapEditor.h"
#include "Modules/m1UndoRedo.h"

#include "Tools/FileSystem.h"

#include "Tools/System/Profiler.h"

#include "ExternalTools/mmgr/mmgr.h"

Application::Application() 
{
}

Application::~Application()	
{
	FileSystem::DeleteRoot();
}

bool Application::Init()
{
	PROFILE_FUNCTION();

	framerate_last_second_timer.Start();

	FileSystem::GenerateFolders();

	//Create instances of modules
	input = new m1Input();
	window = new m1Window();
	render = new m1Render3D();
	scene = new m1Scene();
	gui = new m1GUI();
	objects = new m1Objects();
	camera = new m1Camera3D();
	resources = new m1Resources();
	importer = new m1Importer();
	events = new m1Events();
	map_editor = new m1MapEditor();
	undo = new m1UndoRedo();

	//Assign order of execution to modules NOTE: Inverse order to CleanUp()
	modules.push_back(events);
	modules.push_back(input);
	modules.push_back(window);
	modules.push_back(undo);

	modules.push_back(resources);
	modules.push_back(importer);

	modules.push_back(camera);
	modules.push_back(map_editor);
	modules.push_back(scene);
	modules.push_back(objects);

	modules.push_back(gui);
	modules.push_back(render);

	nlohmann::json conf = FileSystem::OpenJSONFile("prefs/Configuration.json");

	if (conf.is_null()) {
		LOGNE("Configuration.json not found on project folder");
		conf = FileSystem::OpenJSONFile((FileSystem::sAppdata + "/Configuration/Configuration.json").c_str());
		if (conf.is_null()) {
			LOGNE("Configuration.json not found on app folder");
		}
	}

	name.assign(conf["App"].value("name", "PROGRAM"));
	version.assign(conf["App"].value("version", "0.1"));

	for (auto i = modules.begin(); i != modules.end(); ++i) {
		LOGN("Initializing module %s", (*i)->name.c_str());
		(*i)->Init(conf[(*i)->name]);
	}

	sys_info.FillInfo();
	sys_info.SaveInFile();

	return true;
}

bool Application::Start()
{
	PROFILE_FUNCTION();

	for (auto i = modules.begin(); i != modules.end(); ++i) {
		LOGN("Starting module %s", (*i)->name.c_str());
		(*i)->Start();
	}
	return true;
}

void Application::PrepareUpdate()
{
	last_time = time;
	time = SDL_GetPerformanceCounter();
	++frame_count;

	dt = (float)((double)(time - last_time) / (double)SDL_GetPerformanceFrequency());
	framerate = (unsigned char)(1.f / dt);
	if (framerate_last_second_timer.ElapsedMilliseconds() >= 1000) {
		framerate_last_second = framerate;
		framerate_last_second_timer.Start();
	}
}

UpdateStatus Application::Update()
{
	PROFILE_FUNCTION();
	UpdateStatus ret = UpdateStatus::UPDATE_CONTINUE;

	PrepareUpdate();
	
	{
		PROFILE_SECTION("Application::PreUpdate");
		for (auto i = modules.begin(); i != modules.end(); ++i) {
			ret = (*i)->PreUpdate();
			if (ret != UpdateStatus::UPDATE_CONTINUE) {
				LOGN("Module %s returned %s on PreUpdate()", (*i)->name.c_str(), Module::UpdateStatusToString(ret).c_str());
				return ret;
			}
		}
	}
	
	{
		PROFILE_SECTION("Application::Update");
		for (auto i = modules.begin(); i != modules.end(); ++i) {
			ret = (*i)->Update();
			if (ret != UpdateStatus::UPDATE_CONTINUE) {
				LOGN("Module %s returned %s on PreUpdate()", (*i)->name.c_str(), Module::UpdateStatusToString(ret).c_str());
				return ret;
			}
		}
	}
	
	{
		PROFILE_SECTION("Application::PostUpdate");
		for (auto i = modules.begin(); i != modules.end(); ++i) {
			ret = (*i)->PostUpdate();
			if (ret != UpdateStatus::UPDATE_CONTINUE) {
				LOGN("Module %s returned %s on PreUpdate()", (*i)->name.c_str(), Module::UpdateStatusToString(ret).c_str());
				return ret;
			}
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
	PROFILE_FUNCTION();
	bool ret = true;

	for (auto i = modules.rbegin(); i != modules.rend(); ++i) {
		LOGN("Cleaning Up module %s", (*i)->name.c_str());
		ret = (*i)->CleanUp();
		delete* i;
		*i = nullptr;
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

void Application::SaveConfiguration()
{
	nlohmann::json conf;

	conf["App"]["name"] = GetName();
	conf["App"]["version"] = GetVersion();

	for (auto m = modules.begin(); m != modules.end(); ++m) {
		(*m)->Save(conf[(*m)->name]);
	}

	FileSystem::SaveJSONFile("Configuration/Configuration.json", conf);
}

void Application::LoadConfiguration(const char* file)
{
	nlohmann::json conf = FileSystem::OpenJSONFile(file);

	if (conf.is_null()) {
		LOGNW("Configuration on %s not found", file);
		return;
	}

	name.assign(conf["App"]["name"]);
	version.assign(conf["App"]["version"]);

	for (auto m = modules.begin(); m != modules.end(); ++m) {
		(*m)->Load(conf[(*m)->name]);
	}
}
