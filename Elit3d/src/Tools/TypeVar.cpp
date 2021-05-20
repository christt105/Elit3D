#include "Tools/TypeVar.h"

std::string TypeVar::TypeToName(Type t)
{
	switch (t)
	{
	case TypeVar::Type::Float:
		return std::string("float");
	case TypeVar::Type::Int:
		return std::string("int");
	case TypeVar::Type::String:
		return std::string("string");
	case TypeVar::Type::Bool:
		return std::string("bool");
	}
	return std::string("unknown");
}

float TypeVar::fGetValue()
{
	return dynamic_cast<fTypeVar*>(this)->value;
}

int TypeVar::iGetValue()
{
	return dynamic_cast<iTypeVar*>(this)->value;
}

std::string TypeVar::sGetValue()
{
	return dynamic_cast<sTypeVar*>(this)->value;
}

bool TypeVar::bGetValue()
{
	return dynamic_cast<bTypeVar*>(this)->value;
}
