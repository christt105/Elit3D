#pragma once

#include <string>
#include <vector>

#include "int2.h"

#include "ExternalTools/infoware/include/infoware.hpp"

struct SystemInfo
{
private:
	struct Display {
		int resolution[2] = { 0, 0 };
		int dpi = 0;
		int colour_depth = 0;
		int refresh_rate = 0;
	};

public:
	std::string platform;
	std::string platform_fullname;
	std::string platform_version;

	std::vector<Display> displays;

	std::string cpu_name;
	int cpu_cores = 0;
	std::string architecture;
	std::string endianness;

	int ram_mb = 0;

	std::string ogl_version;

	std::string vendor;
	std::string model;

	void FillInfo();
	void SaveInFile() const;
	const char* GetIwareVersion() const;

private:
	//static const char* endianness_name(iware::cpu::endianness_t endianness) noexcept;
	//static const char* architecture_name(iware::cpu::architecture_t architecture) noexcept;
};

