#pragma once
#include "Module.h"

#include <stack>


struct TypeVar {
    enum class Type {
        NONE = -1,
        Float,
        Int,
        String
    };
    Type type = Type::NONE;

    virtual ~TypeVar() {}
};

struct fTypeVar : public TypeVar {
    fTypeVar(float n) :value(n) {
        type = Type::Float;
    }
    float value = 0.f;
};

struct iTypeVar : public TypeVar {
    iTypeVar(int n) :value(n) {
        type = Type::Int;
    }
    int value = 0;
};

struct sTypeVar : public TypeVar {
    sTypeVar(const char* n) :value(n) {
        type = Type::String;
    }

    std::string value;
};

struct Event {
    enum Type {
        NONE = -1,
        TEST,
        FILE_CREATED,
        FILE_REMOVED,
        FILE_RENAMED,
        FILE_MOVED
    };
    Event();
    Event(Type t);
    ~Event();
    Type type = NONE;
    std::unordered_map<std::string, TypeVar*> info;
};

class m1Events :
    public Module
{
public:
    m1Events();
    ~m1Events();

    UpdateStatus PreUpdate() override;

    void AddEvent(Event* e);

private:
    std::stack<Event*> events;
};

