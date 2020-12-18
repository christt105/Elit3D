#include "TypeVar.h"

std::string TypeVar::TypeToName(Type t)
{
	switch (t)
	{
	case TypeVar::Type::Float:
		return std::string("float");
		break;
	case TypeVar::Type::Int:
		return std::string("int");
		break;
	case TypeVar::Type::String:
		return std::string("string");
		break;
	case TypeVar::Type::Bool:
		return std::string("bool");
		break;
	default:
		return std::string("unknown");
		break;
	}
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
