#pragma once
#include "Modules/Base/Module.h"
#include "Tools/TypeVar.h"

#include <stack>
#include <memory>

class m1Events :
    public Module
{
public:
    struct Event {
        enum class Type {
            NONE = -1,

            FILE_CREATED,
            FILE_MODIFIED,
            FILE_REMOVED,
            FILE_RENAMED,
            FILE_MOVED,

            FOLDER_CREATED,
            FOLDER_MODIFIED,
            FOLDER_REMOVED,
            FOLDER_RENAMED,
            FOLDER_MOVED,

            SAVE_CONFIGURATION,
            LOAD_CONFIGURATION,
            LOAD_DEFAULT_CONFIGURATION,

            DELETE_OBJ,

            RESIZE_MAP,
            REORDER_LAYERS,

            SAVE_MAP,
            EXPORT_MAP
        };
        Event();
        Event(Type t);
        Event(Type t, const char* basic_info);
        Event(Type t, int basic_info);
        ~Event();

        Type type = Type::NONE;
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

