#pragma once

#include <string.h>

#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

#define LOG(format, ...) Logger::Log(__FILENAME__, __FUNCTION__, __LINE__, format, __VA_ARGS__);
#define LOGW(format, ...) Logger::WLog(__FILENAME__, __FUNCTION__, __LINE__, format, __VA_ARGS__);
#define LOGE(format, ...) Logger::ELog(__FILENAME__, __FUNCTION__, __LINE__, format, __VA_ARGS__);

class Logger
{
	enum class LogType {
		INFO, WARNING, ERR
	};
public:
	Logger();
	~Logger();

	static void Log(const char file[], const char func[], int line, const char* text, ...);
	static void WLog(const char file[], const char func[], int line, const char* text, ...);
	static void ELog(const char file[], const char func[], int line, const char* text, ...);

private:
	char* txt = nullptr;
};