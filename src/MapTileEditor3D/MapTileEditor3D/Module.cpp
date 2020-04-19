#include "Module.h"

Module::Module(const char* n, bool start_enabled)
    : name(n), enabled(start_enabled)
{
}

Module::~Module()
{
}

bool Module::Init()
{
    return true;
}

bool Module::Start()
{
    return true;
}

UpdateStatus Module::PreUpdate()
{
    return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus Module::Update()
{
    return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus Module::PostUpdate()
{
    return UpdateStatus::UPDATE_CONTINUE;
}

bool Module::CleanUp()
{
    return true;
}
