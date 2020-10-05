#pragma once

#include "ExternalTools/MathGeoLib/include/Math/float3.h"

#ifdef _DEBUG
#define HANDLE_ERROR() oglh::_HandleError(__FUNCTION__)
#else
#define HANDLE_ERROR()
#endif

class oglh
{
public:
	//TODO: add comments
	//Textures
	static void GenTexture(unsigned int& id);
	static void DeleteTexture(unsigned int& id);
	static void ActiveTexture(int val);
	static void BindTexture(unsigned int id);
	static void GenTextureData(unsigned int& id, bool repeat, bool nearest, unsigned int size_x, unsigned int size_y, const unsigned char* data);
	static void SetTextureProperties(unsigned int id, bool repeat, bool nearest);
	static void TexSubImage2D(int x, int y, int width, int height, unsigned char* pixels);
	static void UnBindTexture();
	//Objects
	static void BindBuffers(unsigned int vao, unsigned int vertex, unsigned int elements);
	static void UnBindBuffers();
	static void DrawArrays(int n); //TODO: Allow other geometry
	static void DrawElements(int size); //TODO: Allow other geometry
	static void OldDrawLines(const float3& begin, const float3& end); //TODO: deprecate old direct mode
	static void GenVAO(unsigned int& id);
	static void DeleteVAO(unsigned int& vao, unsigned int& vertex, unsigned int& elements);
	static void GenArrayBuffer(unsigned int& id, unsigned int size, unsigned int type_size, unsigned int element_size, const float* data);
	static void GenArrayBuffer(unsigned int& id, unsigned int size, unsigned int type_size, unsigned int element_size, const float* data, unsigned int attrib_index, unsigned int attrib_size);
	static void GenElementBuffer(unsigned int& id, unsigned int size, unsigned int* data);
	static void DeleteBuffer(unsigned int& id);
	//Misc
	static void PolygonMode(bool line);
	static std::string GetVersion();
	static std::string GetVendor();
	static std::string GetModel();
private:
	static void _HandleError(const char* func);
};

