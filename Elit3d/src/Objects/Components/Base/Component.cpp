#include "Objects/Components/Base/Component.h"

Component::Component(Object* obj, Type type) : object(obj), type(type)
{
}

Component::~Component()
{
}

nlohmann::json Component::Parse()
{
    nlohmann::json n;

    n["type"] = type;

    return n;
}
