#pragma once

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
    sTypeVar(const std::string& n) :value(n) {
        type = Type::String;
    }
    std::string value;
};