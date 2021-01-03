#include "p1About.h"

#include <sstream>

#include "Application.h"
#include "FileSystem.h"
#include "m1GUI.h"
#include "ExternalTools/imgui_markdown/imgui_markdown.h"

#include "OSUtils.h"

#include <SDL_version.h>
#include "OpenGLHelper.h"
#include "ExternalTools/DevIL/include/IL/il.h"
#include "ExternalTools/DevIL/include/IL/ilu.h"
#include "ExternalTools/DevIL/include/IL/ilut.h"
#include "ExternalTools/Assimp/include/version.h"
//#include "ExternalTools/infoware/include/version.hpp"

//TODO: Put markdown code in other place
void LinkCallback(ImGui::MarkdownLinkCallbackData data_);
inline ImGui::MarkdownImageData ImageCallback(ImGui::MarkdownLinkCallbackData data_);

static ImFont* H1 = NULL;
static ImFont* H2 = NULL;
static ImFont* H3 = NULL;

static ImGui::MarkdownConfig mdConfig;


void LinkCallback(ImGui::MarkdownLinkCallbackData data_)
{
	std::string url(data_.link, data_.linkLength);
	if (!data_.isImage)
	{
		ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
	}
}

inline ImGui::MarkdownImageData ImageCallback(ImGui::MarkdownLinkCallbackData data_)
{
	// In your application you would load an image based on data_ input. Here we just use the imgui font texture.
	ImTextureID image = ImGui::GetIO().Fonts->TexID;
	// > C++14 can use ImGui::MarkdownImageData imageData{ true, false, image, ImVec2( 40.0f, 20.0f ) };
	ImGui::MarkdownImageData imageData;
	imageData.isValid = true;
	imageData.useLinkCallback = false;
	imageData.user_texture_id = image;
	imageData.size = ImVec2(40.0f, 20.0f);
	return imageData;
}

void LoadFonts(float fontSize_ = 12.0f)
{
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->Clear();
	// Base font
	io.Fonts->AddFontFromFileTTF("myfont.ttf", fontSize_);
	// Bold headings H2 and H3
	H2 = io.Fonts->AddFontFromFileTTF("myfont-bold.ttf", fontSize_);
	H3 = mdConfig.headingFormats[1].font;
	// bold heading H1
	float fontSizeH1 = fontSize_ * 1.1f;
	H1 = io.Fonts->AddFontFromFileTTF("myfont-bold.ttf", fontSizeH1);
}

void ExampleMarkdownFormatCallback(const ImGui::MarkdownFormatInfo& markdownFormatInfo_, bool start_)
{
	// Call the default first so any settings can be overwritten by our implementation.
	// Alternatively could be called or not called in a switch statement on a case by case basis.
	// See defaultMarkdownFormatCallback definition for furhter examples of how to use it.
	ImGui::defaultMarkdownFormatCallback(markdownFormatInfo_, start_);

	switch (markdownFormatInfo_.type)
	{
		// example: change the colour of heading level 2
	case ImGui::MarkdownFormatType::HEADING:
	{
		if (markdownFormatInfo_.level == 2)
		{
			if (start_)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
			}
			else
			{
				ImGui::PopStyleColor();
			}
		}
		break;
	}
	default:
	{
		break;
	}
	}
}

void Markdown(const std::string& markdown_)
{
	// You can make your own Markdown function with your prefered string container and markdown config.
	// > C++14 can use ImGui::MarkdownConfig mdConfig{ LinkCallback, NULL, ImageCallback, ICON_FA_LINK, { { H1, true }, { H2, true }, { H3, false } }, NULL };
	mdConfig.linkCallback = LinkCallback;
	mdConfig.tooltipCallback = NULL;
	mdConfig.imageCallback = ImageCallback;
	mdConfig.linkIcon = ICON_FA_LINK;
	mdConfig.headingFormats[0] = { H1, true };
	mdConfig.headingFormats[1] = { H2, true };
	mdConfig.headingFormats[2] = { H3, false };
	mdConfig.userData = NULL;
	mdConfig.formatCallback = ExampleMarkdownFormatCallback;
	ImGui::Markdown(markdown_.c_str(), markdown_.length(), mdConfig);
}


p1About::p1About(bool start_enabled, bool appear_mainmenubar, bool can_close)
	: Panel("About", start_enabled, appear_mainmenubar, can_close, ICON_FA_QUESTION_CIRCLE)
{
	std::string file = FileSystem::OpenTextFile("../../Configuration/Text/About.txt"); //TODO: Markdown [https://github.com/juliettef/imgui_markdown]
	std::istringstream iss(file);
	std::string p;
	for (std::string line; std::getline(iss, line); )
	{
		if (line.find("---") != std::string::npos) {
			file_data.push_back(p);
			p.clear();
		}
		else {
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
	ImGui::TextWrapped("%s v%s\n\n", App->GetName(), App->GetVersion());
	Markdown(file_data[0]);

	SDL_version sdl;
	SDL_VERSION(&sdl);
	ImGui::BulletText("SDL: "); ImGui::SameLine(); ImGui::TextColored(ORANGE, "%d.%d.%d", sdl.major, sdl.minor, sdl.patch);
	ImGui::BulletText("OpenGL: "); ImGui::SameLine(); ImGui::TextColored(ORANGE, "%s", oglh::GetVersion().c_str());
	ImGui::BulletText("ImGui: "); ImGui::SameLine(); ImGui::TextColored(ORANGE, "%s", ImGui::GetVersion());
	ImGui::BulletText("ImGui markdown");
	ImGui::BulletText("JSON for Modern C++: "); ImGui::SameLine(); ImGui::TextColored(ORANGE, "%i.%i.%i", NLOHMANN_JSON_VERSION_MAJOR, NLOHMANN_JSON_VERSION_MINOR, NLOHMANN_JSON_VERSION_PATCH);
	ImGui::BulletText("DevIL: "); ImGui::SameLine(); ImGui::TextColored(ORANGE, "%s", ilGetString(IL_VERSION_NUM));
	ImGui::BulletText("Assimp: "); ImGui::SameLine(); ImGui::TextColored(ORANGE, "%u.%u.%u", aiGetVersionMajor(), aiGetVersionMinor(), aiGetVersionRevision());
	ImGui::BulletText("PCG");
	ImGui::BulletText("mmgr");
	ImGui::BulletText("MathGeoLib");
	ImGui::BulletText("infoware: "); ImGui::SameLine(); ImGui::TextColored(ORANGE, "%s", iware::version);
	ImGui::BulletText("cpp-base64");
	ImGui::BulletText("zlib: "); ImGui::SameLine(); ImGui::TextColored(ORANGE, "1.2.11");
	ImGui::BulletText("pugixml: "); ImGui::SameLine(); ImGui::TextColored(ORANGE, "1.10");
	ImGui::BulletText("par_shapes");
	
	Markdown(file_data[1].c_str());

	if (ImGui::CollapsingHeader("License")) {
		ImGui::Text("BSD 2-Clause License\n"
			"\n"
			"Copyright (c) 2020, Christian Martínez de la Rosa (christt105)\n"
			"All rights reserved.\n"
			"\n"
			"Redistribution and use in source and binary forms, with or without\n"
			"modification, are permitted provided that the following conditions are met:\n"
			"\n"
			"1. Redistributions of source code must retain the above copyright notice, this\n"
			"   list of conditions and the following disclaimer.\n"
			"\n"
			"2. Redistributions in binary form must reproduce the above copyright notice,\n"
			"   this list of conditions and the following disclaimer in the documentation\n"
			"   and/or other materials provided with the distribution.\n"
			"\n"
			"THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\"\n"
			"AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE\n"
			"IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE\n"
			"DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE\n"
			"FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL\n"
			"DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR\n"
			"SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER\n"
			"CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,\n"
			"OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE\n"
			"OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n");
	}
}
