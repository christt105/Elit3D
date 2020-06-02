#include "Module.h"

Module::Module(const char* n, bool start_enabled)
    : name(n), enabled(start_enabled)
{
}

Module::~Module()
{
}

bool Module::Init(const nlohmann::json& node)
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

std::string Module::UpdateStatusToString(UpdateStatus us)
{
    switch (us)
    {
    case UpdateStatus::UPDATE_CONTINUE:
        return std::string("Update Continue");
    case UpdateStatus::UPDATE_STOP:
        return std::string("Update Stop");
    case UpdateStatus::UPDATE_ERROR:
        return std::string("Update Error");
    }
    return std::string("NONE");
}
