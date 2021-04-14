#pragma once

#include <string>
#include <unordered_map>

#include "ExternalTools/ImGui/imgui.h"

#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

#define LOG(format,   ...) Logger::Log(0, __FILENAME__, __FUNCTION__, __LINE__, format, __VA_ARGS__);
#define LOGW(format,  ...) Logger::Log(1, __FILENAME__, __FUNCTION__, __LINE__, format, __VA_ARGS__);
#define LOGE(format,  ...) Logger::Log(2, __FILENAME__, __FUNCTION__, __LINE__, format, __VA_ARGS__);

#define LOGN(format,  ...) Logger::Log(3, __FILENAME__, __FUNCTION__, __LINE__, format, __VA_ARGS__);
#define LOGNW(format, ...) Logger::Log(4, __FILENAME__, __FUNCTION__, __LINE__, format, __VA_ARGS__);
#define LOGNE(format, ...) Logger::Log(5, __FILENAME__, __FUNCTION__, __LINE__, format, __VA_ARGS__);

struct LineLog;

class Logger
{
	friend struct LineLog;
public:
	enum class LogType {
		INFO, WARNING, ERR, ENGINE
	};

	Logger() = default;
	~Logger() = default;

	static void Log(int type, const char file[], const char func[], int line, const char* text, ...);

	static void ExportLog();

	static const char* GetLog();

private:
	static ImGuiTextBuffer txt;
};

struct LineLog {
	LineLog(Logger::LogType t, const char* str, const tm* time);

	Logger::LogType type = Logger::LogType::INFO;
	std::string last_line;
	std::string identifier;
	ImGuiTextBuffer buffer;
	int timestamp[3] = { 0,0,0 };
	unsigned int references = 1U;
	bool opened = false;
};