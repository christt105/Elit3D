#include "Exporter.h"

std::string Exporter::SetVertex(float x, float y, float z)
{
	return "v " + std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(z) + "\n";
}

std::string Exporter::SetVertexTexture(float x, float y)
{
	return "vt " + std::to_string(x) + " " + std::to_string(y) + "\n";
}

std::string Exporter::SetVertexNormal(float x, float y, float z)
{
	return "vn " + std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(z) + "\n";
}

std::string Exporter::SetFace(int x, int y, int z)
{
	return "f " + std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(z) + "\n";
}

std::string Exporter::SetFace(int v1, int v2, int v3, int vt1, int vt2, int vt3)
{
	return "f " + std::to_string(v1) + "/" + std::to_string(vt1) + " " + std::to_string(v2) + "/" + std::to_string(vt2) + " " + std::to_string(v3) + "/" + std::to_string(vt3) + "\n";
}
