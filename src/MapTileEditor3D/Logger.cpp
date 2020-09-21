#include "Logger.h"

#include <Windows.h>
#include <ctime>

#include <iostream>
#include <fstream>

#include "Application.h"
#include "m1GUI.h"
#include "p1Console.h"
#include "FileSystem.h"

#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS

ImGuiTextBuffer Logger::txt;

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

	std::time_t t = std::time(nullptr);
	tm* time = std::localtime(&t);

	
	va_start(ap, format);
	vsprintf_s(tmp_string, 4096, format, ap);
	va_end(ap);

	std::string strtype;
	switch (i) {
	case 4:
		strtype.append("[E]");
	case 1:
		strtype.append("[WARN]");
		break;
	case 5:
		strtype.append("[E]");
	case 2:
		strtype.append("[ERROR]");
		break;
	case 3:
		strtype.append("[E]");
	default:
		strtype.append("[INFO]");
		break;
	}

	if (i < 3 && App && App->gui && App->gui->console) {
		LineLog* log = new LineLog(Logger::LogType::INFO, tmp_string, time);

		switch (i) {
		case 1:
			log->type = Logger::LogType::WARNING;
			break;
		case 2:
			log->type = Logger::LogType::ERR;
			break;
		}

		App->gui->console->logs.push_back(log);
		auto iterator_log = App->gui->console->map_logs.find(file + std::to_string(line));
		if (iterator_log != App->gui->console->map_logs.end()) {
			(*iterator_log).second->buffer.append(((*iterator_log).second->last_line + "\n").c_str());
			(*iterator_log).second->last_line.assign(tmp_string);
			(*iterator_log).second->references++;
		}
		else
			App->gui->console->map_logs[file + std::to_string(line)] = log;
	}
	

	sprintf_s(tmp_string2, 4096, "%02i:%02i:%02i %s\t%s | %s(%d) : %s\n", time->tm_hour, time->tm_min, time->tm_sec, strtype.c_str(), file, func, line, tmp_string);

	txt.append(tmp_string2);

	OutputDebugString(tmp_string2); //Log in IDE Output window
}

void Logger::ExportLog()
{
	std::ofstream file;
	if (!FileSystem::Exists("Export/"))
		FileSystem::CreateFolder("Export/");
	file.open("Export/Log.txt");
	
	file << txt.c_str();

	file.close();
}

const char* Logger::GetLog()
{
	return txt.c_str();
}

LineLog::LineLog(Logger::LogType t, const char* str, const tm* tim)
	: type(t), last_line(str), identifier(str), moment{tim->tm_hour,tim->tm_min,tim->tm_sec}
{
}
