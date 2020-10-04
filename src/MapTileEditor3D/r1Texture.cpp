#include "r1Texture.h"

#include "Application.h"
#include "FileSystem.h"

#include "m1Resources.h"

#include "ExternalTools/DevIL/il.h"
#include "ExternalTools/DevIL/ilut.h"

#include "OpenGLHelper.h"

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
	ILuint image_id = ilGenImage();
	ilBindImage(image_id);

	ilutRenderer(ILUT_OPENGL);

	if (ilLoad(IL_PNG, path.c_str())) { // TODO: load for different types
		width = ilGetInteger(IL_IMAGE_WIDTH);
		height = ilGetInteger(IL_IMAGE_HEIGHT);

		id = ilutGLBindTexImage();
		
		oglh::SetTextureProperties(id, true, true);
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
	unsigned char bits[3];
	bits[0] = r;
	bits[1] = g;
	bits[2] = b;
	glTexSubImage2D(GL_TEXTURE_2D, 0, col, row, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, bits);
	ilEnable(IL_FILE_OVERWRITE);
	if (!ilutGLSaveImage((char*)"./Assets/Maps/testtilemap.png", id)) {
		LOGE("Failed to load texture %s, error: %s", name.c_str(), ilGetString(ilGetError()));
	}
	Unbind();
}
