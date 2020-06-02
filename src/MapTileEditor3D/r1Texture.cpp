#include "r1Texture.h"

#include <GL/glew.h>

#include "Application.h"
#include "FileSystem.h"

#include "m1Resources.h"

#include "ExternalTools/Assimp/include/texture.h"

#include "ExternalTools/DevIL/il.h"
#include "ExternalTools/DevIL/ilut.h"

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
	ILuint image_id = 0;
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &image_id);
	glBindTexture(GL_TEXTURE_2D, image_id);

	ilutRenderer(ILUT_OPENGL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	if (ilLoad(IL_PNG, library_path.c_str())) { // TODO: load for different types
		width = ilGetInteger(IL_IMAGE_WIDTH);
		height = ilGetInteger(IL_IMAGE_HEIGHT);
		id = ilutGLBindTexImage();

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		--references;
		LOGE("Failed to load texture %s, error: %s", name.c_str(), ilGetString(ilGetError()));
	}

	glBindTexture(GL_TEXTURE_2D, NULL);

	ilDeleteImages(1, &image_id);
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

void r1Texture::GenerateFiles(const aiTexture* texture)
{
	
}
