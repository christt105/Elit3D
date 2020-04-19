#include "Logger.h"
#include <Windows.h>
#include <stdio.h>

Logger::Logger()
{
}

Logger::~Logger()
{
}

void Logger::Log(const char file[], const char func[], int line, const char* format, ...)
{
	static char tmp_string[4096];
	static char tmp_string2[4096];
	static va_list  ap;

	va_start(ap, format);
	vsprintf_s(tmp_string, 4096, format, ap);
	va_end(ap);
	sprintf_s(tmp_string2, 4096, "[INFO]\t\tfile: (%s) func: (%s) line: (%d) : %s\n", file, func, line, tmp_string);

	OutputDebugString(tmp_string2); //Log in IDE Output window
}

void Logger::WLog(const char file[], const char func[], int line, const char* format, ...)
{
	static char tmp_string[4096];
	static char tmp_string2[4096];
	static va_list  ap;

	va_start(ap, format);
	vsprintf_s(tmp_string, 4096, format, ap);
	va_end(ap);
	sprintf_s(tmp_string2, 4096, "[WARNING]\tfile: (%s) func: (%s) line: (%d) : %s\n", file, func, line, tmp_string);

	OutputDebugString(tmp_string2); //Log in IDE Output window
}

void Logger::ELog(const char file[], const char func[], int line, const char* format, ...)
{
	static char tmp_string[4096];
	static char tmp_string2[4096];
	static va_list  ap;

	va_start(ap, format);
	vsprintf_s(tmp_string, 4096, format, ap);
	va_end(ap);
	sprintf_s(tmp_string2, 4096, "[ERROR]\t\tfile: (%s) func: (%s) line: (%d) : %s\n", file, func, line, tmp_string);

	OutputDebugString(tmp_string2); //Log in IDE Output window
}

