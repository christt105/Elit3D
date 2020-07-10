#pragma once
#include "Module.h"
#include "TypeVar.h"

#include <stack>
#include <memory>

class m1Events :
    public Module
{
public:
    struct Event {
        enum Type {
            NONE = -1,
            FILE_CREATED,
            FILE_MODIFIED,
            FILE_REMOVED,
            FILE_RENAMED,
            FILE_MOVED
        };
        Event();
        Event(Type t, const char* basic_info = NULL);
        ~Event();

        Type type = NONE;
        std::unordered_map<std::string, TypeVar*> info;
    };

public:
    m1Events();
    ~m1Events();

    UpdateStatus PreUpdate() override;

    void AddEvent(Event* e);

private:
    std::stack<Event*> events;
};

