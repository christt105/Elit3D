#include "Logger.h"

#include <Windows.h>

#include <ctime>

#include <iostream>
#include <fstream>

#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS

std::string Logger::txt;

Logger::Logger()
{
}

Logger::~Logger()
{
}

void Logger::Log(int i, const char file[], const char func[], int line, const char* format, ...)
{
	static char tmp_string[4096];
	static char tmp_string2[4096];
	static va_list  ap;

	va_start(ap, format);
	vsprintf_s(tmp_string, 4096, format, ap);
	va_end(ap);

	static std::string type;
	switch (i) {
	case 1:
		type.assign("[WARN]");
		break;
	case 2:
		type.assign("[ERROR]");
		break;
	default:
		type.assign("[INFO]");
		break;
	}

	std::time_t t = std::time(nullptr);
	tm* time = std::localtime(&t);

	sprintf_s(tmp_string2, 4096, "%i:%i:%s %s\t%s | %s(%d) : %s\n", time->tm_hour, time->tm_min, (time->tm_sec < 10) ? std::string(std::to_string(0) + std::to_string(time->tm_sec)).c_str() : std::to_string(time->tm_sec).c_str(),type.c_str(), file, func, line, tmp_string);

	txt.append(tmp_string2);

	OutputDebugString(tmp_string2); //Log in IDE Output window
}

void Logger::ExportLog()
{
	std::ofstream file;
	file.open("Log.txt");
	file << txt;
	file.close();
}
