#pragma once

struct TypeVar {
    enum class Type {
        NONE = -1,
        Float,
        Int,
        String,
        Bool
    };
    Type type = Type::NONE;

    TypeVar() {}
    TypeVar(Type t) : type(t) {}
    virtual ~TypeVar() {}
};

struct fTypeVar : public TypeVar {
    fTypeVar(float n) : value(n), TypeVar(Type::Int) {}
    float value = 0.f;
};

struct iTypeVar : public TypeVar {
    iTypeVar(int n) : value(n), TypeVar(Type::Int) {}
    int value = 0;
};

struct bTypeVar : public TypeVar {
    bTypeVar(bool n) : value(n), TypeVar(Type::Bool) {}
    bool value = false;
};

struct sTypeVar : public TypeVar {
    sTypeVar() : TypeVar(Type::String) {}
    sTypeVar(const char* n) : value(n), TypeVar(Type::String) {}
    sTypeVar(const std::string& n) : value(n), TypeVar(Type::String) {}

    std::string value;
};