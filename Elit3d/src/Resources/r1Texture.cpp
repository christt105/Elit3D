#include "Resources/r1Texture.h"

#include "Core/Application.h"
#include "Tools/FileSystem.h"

#include "Modules/m1Resources.h"

#include "ExternalTools/DevIL/include/IL/il.h"
#include "ExternalTools/DevIL/include/IL/ilut.h"

#include "Tools/OpenGL/OpenGLHelper.h"

#include "Tools/System/Logger.h"

#include "ExternalTools/mmgr/mmgr.h"

r1Texture::r1Texture(const uint64_t& uid) : Resource(Resource::Type::Texture, uid)
{
}

r1Texture::~r1Texture()
{
}

void r1Texture::Load()
{
	glEnable(GL_TEXTURE_2D);
	ILuint image_id = ilGenImage();
	ilBindImage(image_id);

	ilutRenderer(ILUT_OPENGL);
	
	//if (ilLoad(IL_PNG, path.c_str())) { // TODO: load for different types
	if (iluLoadImage(path.c_str())) { // TODO: load for different types
		width = ilGetInteger(IL_IMAGE_WIDTH);
		height = ilGetInteger(IL_IMAGE_HEIGHT);
		
		id = ilutGLBindTexImage();
		
		oglh::SetTextureProperties(id, wrap, filter);
	}
	else {
		--references;
		LOGE("Failed to load texture %s, error: %s", name.c_str(), ilGetString(ilGetError()));
	}

	glBindTexture(GL_TEXTURE_2D, NULL);

	ilDeleteImages(1, &image_id);
}

void r1Texture::LoadVars()
{
	nlohmann::json meta = FileSystem::OpenJSONFile((path + ".meta").c_str());

	if (meta.find("properties") != meta.end()) {
		tileset = meta["properties"].value("tileset", false);
		wrap = meta["properties"].value("wrap", oglh::Wrap::Repeat);
		filter = meta["properties"].value("filter", oglh::Filter::Nearest);
	}
}

void r1Texture::Unload()
{
	glDeleteTextures(1, &id);
	width = 0;
	height = 0;
	channels = 0;
	id = 0;
}

unsigned int r1Texture::GetBufferID()
{
	return id;
}

unsigned int r1Texture::GetWidth()
{
	return width;
}

unsigned int r1Texture::GetHeight()
{
	return height;
}

void r1Texture::OnInspector()
{
	ImGui::Text("Name: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.f, 0.6f, 0.6f, 1.f), name.c_str());

	ImGui::Checkbox("Is a Tileset", &tileset);
	static const char* wrap_str [4] = { "ClampToEdge","ClampToBorder", "Repeat", "MirroredRepeat" };
	if (ImGui::BeginCombo("Wrap", wrap_str[wrap])) {
		for (int i = 0; i < 4; ++i) {
			if (ImGui::Selectable(wrap_str[i], wrap == i)) {
				wrap = (oglh::Wrap)i;
			}
		}
		ImGui::EndCombo();
	}

	static const char* filter_str[2] = { "Nearest","Linear" };
	if (ImGui::BeginCombo("Filter", filter_str[filter])) {
		for (int i = 0; i < 2; ++i) {
			if (ImGui::Selectable(filter_str[i], filter == i)) {
				filter = (oglh::Filter)i;
			}
		}
		ImGui::EndCombo();
	}

	if (ImGui::Button("Apply")) {
		if (references > 0) {
			Unload();
			Load();
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Save")) {
		nlohmann::json meta = FileSystem::OpenJSONFile((path + ".meta").c_str());
		meta["properties"]["tileset"] = tileset;
		meta["properties"]["wrap"] = wrap;
		meta["properties"]["filter"] = filter;
		FileSystem::SaveJSONFile((path + ".meta").c_str(), meta);
	}
}

void r1Texture::GenerateFiles(const aiTexture* texture)
{
	
}

void r1Texture::Bind()
{
	glBindTexture(GL_TEXTURE_2D, id);
}

void r1Texture::Unbind()
{
	glBindTexture(GL_TEXTURE_2D, NULL);
}

void r1Texture::Edit(int row, int col, int r, int g, int b)
{
	Bind();
	unsigned char bits[3] = { r, g, b };
	glTexSubImage2D(GL_TEXTURE_2D, 0, col, row, 1, 1, GL_RGB8, GL_UNSIGNED_BYTE, bits);
	ilEnable(IL_FILE_OVERWRITE);
	if (!ilutGLSaveImage((char*)"./Assets/Maps/testtilemap.png", id)) {
		LOGE("Failed to load texture %s, error: %s", name.c_str(), ilGetString(ilGetError()));
	}
	Unbind();
}
