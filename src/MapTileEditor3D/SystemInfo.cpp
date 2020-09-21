#include "SystemInfo.h"

#include <SDL_cpuinfo.h>

#include "OpenGLHelper.h"
#include "FileSystem.h"

#include <iostream>

#if defined(_WIN64)
#ifdef _DEBUG
#pragma comment(lib, "ExternalTools/infoware/libx64/_Debug/infowared.lib")
#else
#pragma comment(lib, "ExternalTools/infoware/libx64/_Release/infoware.lib")
#endif
#else
#ifdef _DEBUG
#pragma comment(lib, "ExternalTools/infoware/libx86/_Debug/infowared.lib")
#else
#pragma comment(lib, "ExternalTools/infoware/libx86/_Release/infoware.lib")
#endif
#endif

void SystemInfo::FillInfo()
{
	//platform
	const auto OS_info = iware::system::OS_info();
	platform = OS_info.name;
	platform_fullname = OS_info.full_name;
	platform_version.assign(
		std::to_string(OS_info.major) + '.' + 
		std::to_string(OS_info.minor) + '.' + 
		std::to_string(OS_info.patch) + " build " + std::to_string(OS_info.build_number)
		);

	//cpu
	cpu_name = iware::cpu::model_name();
	cpu_cores = SDL_GetCPUCount();
	ram_mb = SDL_GetSystemRAM();
	architecture = architecture_name(iware::cpu::architecture());
	endianness = endianness_name(iware::cpu::endianness());

	//gpu
	ogl_version = oglh::GetVersion();
	vendor = oglh::GetVendor();
	model = oglh::GetModel();

	const auto ds = iware::system::displays();
	if (!ds.empty())
		for (auto i = 0u; i < ds.size(); ++i) {
			const auto& display = ds[i];
			Display d;
			d.resolution[0] = display.width; d.resolution[1] = display.height;
			d.dpi = display.dpi;
			d.colour_depth = display.bpp;
			d.refresh_rate = (int)display.refresh_rate;
			displays.push_back(d);
		};

}

void SystemInfo::SaveInFile() const
{
	std::string display;
	for (auto i = displays.begin(); i != displays.end(); ++i) {
		display.append(
			"\n\tDisplay #" + std::to_string(i - displays.begin()) + 
			"\n\t\tResolution: " 
			+ std::to_string((*i).resolution[0]) + 'x'
			+ std::to_string((*i).resolution[1]) + 
			"\n\t\tDPI: " + std::to_string((*i).dpi) +
			"\n\t\tColor Depth: " + std::to_string((*i).colour_depth) +
			"\n\t\tRefresh Rate: " + std::to_string((*i).refresh_rate) + "Hz\n"
		);
		
	}

	std::string file = {
		"System and Hardware information by infoware v" + std::string(iware::version) +

		"\n\nOS:\n" + 
		"\tName: " + platform +
		"\n\tFull Name: " + platform_fullname +
		"\n\tVersion: " + platform_version +

		"\nCPU:\n" +
		"\tModel: " + cpu_name +
		"\n\tNumber Cores: " + std::to_string(cpu_cores) +
		"\n\tArchitecture: " + architecture +
		"\n\tEndianness: " + endianness +
		
		"\n\nRAM: " + std::to_string(ram_mb) + "MB\n\n" +

		"GPU:\n" +
		"\tOpenGL version: " + ogl_version +
		"\n\tGraphics Card:\n\t\tVendor: " + vendor + "\n\t\tModel: " + model + 

		"\nDisplays:\n" + display
	};

	if (!FileSystem::Exists("Export/"))
		FileSystem::CreateFolder("Export/");
	FileSystem::SaveTextFile("Export/SystemInfo.txt", file.c_str());
}

const char* SystemInfo::GetIwareVersion() const
{
	return iware::version;
}

const char* SystemInfo::endianness_name(iware::cpu::endianness_t endianness) noexcept
{
	switch (endianness) {
	case iware::cpu::endianness_t::little:
		return "Little-Endian";
	case iware::cpu::endianness_t::big:
		return "Big-Endian";
	default:
		return "Unknown";
	}
}

const char* SystemInfo::architecture_name(iware::cpu::architecture_t architecture) noexcept
{
	switch (architecture) {
	case iware::cpu::architecture_t::x64:
		return "x64";
	case iware::cpu::architecture_t::arm:
		return "ARM";
	case iware::cpu::architecture_t::itanium:
		return "Itanium";
	case iware::cpu::architecture_t::x86:
		return "x86";
	default:
		return "Unknown";
	}
}
