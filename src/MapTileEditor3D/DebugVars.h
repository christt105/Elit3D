#pragma once

struct DebugVars {
	bool draw_mouse_pick_line = false;
};

struct SystemInfo {
	std::string platform;
	int cpu_cores = 0;
	int ram_mb = 0;
	std::string ogl_version;
	std::string vendor;
	std::string model;
};