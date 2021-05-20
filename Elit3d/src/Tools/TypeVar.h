#pragma once

#include <string>

struct TypeVar {
    enum class Type {
        NONE = -1,
        Int,
        String,
        Float,
        Bool
    };
    Type type = Type::NONE;

    TypeVar(Type t) : type(t) {}
    virtual ~TypeVar() = default;

    float fGetValue();
    int iGetValue();
    std::string sGetValue();
    bool bGetValue();

    virtual std::string ToString() = 0;
    static std::string TypeToName(Type t);
};

struct fTypeVar : public TypeVar {
    fTypeVar(float n) : value(n), TypeVar(Type::Float) {}
    std::string ToString() override { return std::to_string(value); }
    float value = 0.f;
};

struct iTypeVar : public TypeVar {
    iTypeVar(int n) : value(n), TypeVar(Type::Int) {}
    std::string ToString() override { return std::to_string(value); }
    int value = 0;
};

struct bTypeVar : public TypeVar {
    bTypeVar(bool n) : value(n), TypeVar(Type::Bool) {}
    std::string ToString() override { return value ? "true" : "false"; }
    bool value = false;
};

struct sTypeVar : public TypeVar {
    sTypeVar() : TypeVar(Type::String) {}
    sTypeVar(const char* n) : value(n), TypeVar(Type::String) {}
    sTypeVar(const std::string& n) : value(n), TypeVar(Type::String) {}

    std::string ToString() override { return value; }

    std::string value;
};
