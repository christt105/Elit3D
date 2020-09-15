#pragma once

#include "ExternalTools/MathGeoLib/include/Math/float3.h"

class oglh
{
public:
	//TODO: add comments
	static void GenTexture(unsigned int& id);
	static void HandleError();
	static void ActiveTexture(int val);
	static void BindTexture(unsigned int id);
	static void UnBindTexture();
	static void UnBindBuffers();
	static void DrawArrays(int n); //TODO: Allow other geometry
	static void DrawElements(int size); //TODO: Allow other geometry
	static void OldDrawLines(const float3& begin, const float3& end); //TODO: deprecate old direct mode
	static void BindBuffers(unsigned int vao, unsigned int vertex, unsigned int elements);
	static void DeleteBuffer(unsigned int& id);
	static void DeleteVAO(unsigned int& vao, unsigned int& vertex, unsigned int& elements);
	static void GenVAO(unsigned int& id);
	static void GenArrayBuffer(unsigned int& id, unsigned int size, unsigned int type_size, unsigned int element_size, const float* data);
	static void GenArrayBuffer(unsigned int& id, unsigned int size, unsigned int type_size, unsigned int element_size, const float* data, unsigned int attrib_index, unsigned int attrib_size);
	static void GenElementBuffer(unsigned int& id, unsigned int size, unsigned int* data);
	static void GenTextureData(unsigned int& id, bool repeat, bool nearest, unsigned int size_x, unsigned int size_y, unsigned char* data);
	static void SetTextureProperties(unsigned int id, bool repeat, bool nearest);
};

