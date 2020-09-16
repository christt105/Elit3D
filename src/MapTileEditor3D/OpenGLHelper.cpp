#include "OpenGLHelper.h"

#include <GL/glew.h>

#include "Logger.h"

void oglh::GenTexture(unsigned int& id)
{
	glGenTextures(1, &id);
}

void oglh::_HandleError(const char* func)
{
	GLenum err = glGetError();
	while (err != GL_NO_ERROR) {
		std::string error_type;
		switch (err)
		{
		case GL_INVALID_ENUM:
			error_type.assign("GL_INVALID_ENUM");
			break;
		case GL_INVALID_VALUE:
			error_type.assign("GL_INVALID_VALUE");
			break;
		case GL_INVALID_OPERATION:
			error_type.assign("GL_INVALID_OPERATION");
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			error_type.assign("GL_INVALID_FRAMEBUFFER_OPERATION");
			break;
		case GL_OUT_OF_MEMORY:
			error_type.assign("GL_OUT_OF_MEMORY");
			break;
		case GL_STACK_UNDERFLOW:
			error_type.assign("GL_STACK_UNDERFLOW");
			break;
		case GL_STACK_OVERFLOW:
			error_type.assign("GL_STACK_OVERFLOW");
			break;
		default:
			break;
		}
		LOGW("OpenGL error %i (%s) on %s", err, error_type.c_str(), func);
		err = glGetError();
	}
}

void oglh::ActiveTexture(int val)
{
	glActiveTexture(GL_TEXTURE0 + val);
}

void oglh::BindTexture(unsigned int id)
{
	glBindTexture(GL_TEXTURE_2D, id);
	HANDLE_ERROR();
}

void oglh::TexSubImage2D(int x, int y, int width, int height, unsigned char* pixels)
{
	glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	HANDLE_ERROR();
}

void oglh::UnBindTexture()
{
	glBindTexture(GL_TEXTURE_2D, 0u);
}

void oglh::UnBindBuffers()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void oglh::DrawArrays(int n)
{
	glDrawArrays(GL_TRIANGLES, 0, n);
}

void oglh::DrawElements(int size)
{
	glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, (void*)0);
}

void oglh::OldDrawLines(const float3& begin, const float3& end)
{
	glBegin(GL_LINES);
	glVertex3f(begin.x, begin.y, begin.z);
	glVertex3f(end.x, end.y, end.z);
	glEnd();
}

void oglh::BindBuffers(unsigned int vao, unsigned int vertex, unsigned int elements)
{
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vertex);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elements);
}

void oglh::DeleteBuffer(unsigned int& id)
{
	glDeleteBuffers(1, &id);
}

void oglh::DeleteVAO(unsigned int& vao, unsigned int& vertex, unsigned int& elements)
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vertex);
	glDeleteBuffers(1, &elements);
}

void oglh::DeleteTexture(unsigned int& id)
{
	glDeleteTextures(1, &id);
}

void oglh::GenVAO(unsigned int& vao)
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
}

void oglh::GenArrayBuffer(unsigned int& id, unsigned int size, unsigned int type_size, unsigned int element_size, const float* data)
{
	glGenBuffers(1, &id);
	glBindBuffer(GL_ARRAY_BUFFER, id);
	glBufferData(GL_ARRAY_BUFFER, type_size * size * element_size, data, GL_STATIC_DRAW);
}

void oglh::GenArrayBuffer(unsigned int& id, unsigned int size, unsigned int type_size, unsigned int element_size, const float* data, unsigned int attrib_index, unsigned int attrib_size)
{
	glGenBuffers(1, &id);
	glBindBuffer(GL_ARRAY_BUFFER, id);
	glBufferData(GL_ARRAY_BUFFER, type_size * size * element_size, data, GL_STATIC_DRAW);
	glVertexAttribPointer(attrib_index, attrib_size, GL_FLOAT, GL_FALSE, 0, (void*)0); // TODO: allow other than GL_FLOAT
	glEnableVertexAttribArray(attrib_index);
}

void oglh::GenElementBuffer(unsigned int& id, unsigned int size, unsigned int* data)
{
	glGenBuffers(1, &id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * size, data, GL_STATIC_DRAW);
}

void oglh::GenTextureData(unsigned int& id, bool repeat, bool nearest, unsigned int size_x, unsigned int size_y, unsigned char* data)
{
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	
	if (repeat) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //TODO: Allow more
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	if (nearest) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, size_x, size_y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenerateMipmap(GL_TEXTURE_2D);

	HANDLE_ERROR();
}

void oglh::SetTextureProperties(unsigned int id, bool repeat, bool nearest)
{
	glBindTexture(GL_TEXTURE_2D, id);

	if (repeat) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //TODO: Allow more
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	if (nearest) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenerateMipmap(GL_TEXTURE_2D);

	HANDLE_ERROR();
}
