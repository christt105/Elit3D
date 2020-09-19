#include "p1About.h"

#include <sstream>

#include "Application.h"
#include "FileSystem.h"
#include "m1GUI.h"

#include <SDL_version.h>
#include "OpenGLHelper.h"
#include "ExternalTools/DevIL/il.h"
#include "ExternalTools/DevIL/ilu.h"
#include "ExternalTools/DevIL/ilut.h"
#include "ExternalTools/Assimp/include/version.h"
#include "ExternalTools/infoware/include/version.hpp"


p1About::p1About(bool start_enabled, bool appear_mainmenubar, bool can_close)
	: Panel("About", start_enabled, appear_mainmenubar, can_close, ICON_FA_QUESTION_CIRCLE)
{
	std::string file = FileSystem::OpenTextFile("Configuration/Text/About.txt");
	std::istringstream iss(file);
	int i = 0;
	std::string p;
	bool start = false;
	for (std::string line; std::getline(iss, line); )
	{
		if (line.find("#" + std::to_string(i)) != std::string::npos) {
			if (i++ == 0) {
				start = true;
			}
			else {
				file_data.push_back(p);
				p.clear();
			}
		}
		else {
			if (start)
				p.append(line + '\n');
		}
	}

	if (!p.empty()) {
		file_data.push_back(p);
	}
}

p1About::~p1About()
{
}

void p1About::Update()
{
	ImGui::TextWrapped("%s v.%s\n\n", App->GetName(), App->GetVersion()); 
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Made by "); ImGui::SameLine();
	if (ImGui::Button(ICON_FA_GITHUB" christt105", ImVec2(0.f, 25.f))) {
		App->ExecuteURL("https://github.com/christt105");
	}
	
	ImGui::TextWrapped("Licensed under CC-BY-4.0, see LICENSE file.");
	ImGui::TextWrapped(file_data[0].c_str());
	ImGui::Spacing();
	if (ImGui::Button("Repository")) {
		App->ExecuteURL("https://github.com/christt105/MapTileEditor-3D");
	}
	ImGui::SameLine();
	if (ImGui::Button("Wiki")) {
		App->ExecuteURL("https://github.com/christt105/MapTileEditor-3D/wiki");
	}

	ImGui::TextWrapped(file_data[1].c_str());
	SDL_version sdl;
	SDL_VERSION(&sdl);
	ImGui::BulletText("SDL: "); ImGui::SameLine(); ImGui::TextColored(ORANGE, "%d.%d.%d", sdl.major, sdl.minor, sdl.patch);
	ImGui::BulletText("OpenGL: "); ImGui::SameLine(); ImGui::TextColored(ORANGE, "%s", oglh::GetVersion().c_str());
	ImGui::BulletText("ImGui: "); ImGui::SameLine(); ImGui::TextColored(ORANGE, "%s", ImGui::GetVersion());
	ImGui::BulletText("JSON for Modern C++: "); ImGui::SameLine(); ImGui::TextColored(ORANGE, "%i.%i.%i", NLOHMANN_JSON_VERSION_MAJOR, NLOHMANN_JSON_VERSION_MINOR, NLOHMANN_JSON_VERSION_PATCH);
	ImGui::BulletText("DevIL: "); ImGui::SameLine(); ImGui::TextColored(ORANGE, "%s", ilGetString(IL_VERSION_NUM));
	ImGui::BulletText("Assimp: "); ImGui::SameLine(); ImGui::TextColored(ORANGE, "%u.%u.%u", aiGetVersionMajor(), aiGetVersionMinor(), aiGetVersionRevision());
	ImGui::BulletText("PCG: "); ImGui::SameLine(); ImGui::TextColored(ORANGE, "0.98.1");
	ImGui::BulletText("mmgr: "); ImGui::SameLine(); ImGui::TextColored(ORANGE, "1.0.0");
	ImGui::BulletText("MathGeoLib: "); ImGui::SameLine(); ImGui::TextColored(ORANGE, "1.5");
	ImGui::BulletText("infoware: "); ImGui::SameLine(); ImGui::TextColored(ORANGE, "%s", iware::version);
	ImGui::BulletText("par_shapes: "); ImGui::SameLine(); ImGui::TextColored(ORANGE, "1.0");
	
	ImGui::TextWrapped(file_data[2].c_str());
	if (ImGui::Button("christt105.github.io")) {
		App->ExecuteURL("https://christt105.github.io");
	}
	ImGui::TextWrapped("Also, you can visit my dev-blog if you are interested in the development of the project:");
	if (ImGui::Button("christt105.github.io/dev-blog")) {
		App->ExecuteURL("https://christt105.github.io/dev-blog");
	}
}
