#include "r1Texture.h"

#include <GL/glew.h>

#include "Application.h"
#include "FileSystem.h"

#include "m1Resources.h"

#include "ExternalTools/Assimp/include/texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Logger.h"

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
	glGenBuffers(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	unsigned char* data = stbi_load("Assets/Tilesets/four_tiles.png", &width, &height, &channels, STBI_rgb_alpha);

	if (data) {

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		if (channels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		else if (channels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		--references;
		LOGE("Failed to load texture %s", name.c_str());
	}

	stbi_image_free(data);
}

void r1Texture::Unload()
{
	glDeleteBuffers(1, &id);
	width = 0;
	height = 0;
	channels = 0;
}

unsigned int r1Texture::GetBufferID()
{
	return id;
}

void r1Texture::GenerateFiles(const aiTexture* texture)
{
	
}
