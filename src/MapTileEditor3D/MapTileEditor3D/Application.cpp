#include "Application.h"

Application::Application() {

}

Application::~Application()	{

}

bool Application::Init()
{
	return true;
}

bool Application::Start()
{
	return true;
}

int Application::Update()
{
	return 0;
}

bool Application::CleanUp()
{
	return true;
}

void Application::AddModule(Module * const m)
{
	modules.push_back(m);
}
