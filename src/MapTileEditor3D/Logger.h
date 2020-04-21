#pragma once

#include <string>

#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

#define LOG(format, ...)  Logger::Log(0, __FILENAME__, __FUNCTION__, __LINE__, format, __VA_ARGS__);
#define LOGW(format, ...) Logger::Log(1, __FILENAME__, __FUNCTION__, __LINE__, format, __VA_ARGS__);
#define LOGE(format, ...) Logger::Log(2, __FILENAME__, __FUNCTION__, __LINE__, format, __VA_ARGS__);

class Logger
{
	enum class LogType {
		INFO, WARNING, ERR
	};
public:
	Logger();
	~Logger();

	static void Log(int type, const char file[], const char func[], int line, const char* text, ...);

	static void ExportLog();

private:
	static std::string txt;
};