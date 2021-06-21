#pragma once
#include <string>

namespace Exporter {
	//TODO Create Class auto export
	std::string SetVertex(float x, float y, float z);
	std::string SetVertexTexture(float x, float y);
	std::string SetVertexNormal(float x, float y, float z);
	std::string SetFace(int x, int y, int z);
	std::string SetFace(int v1, int v2, int v3, int vt1, int vt2, int vt3);
};