#include "Exporter.h"

std::string Exporter::SetVertex(float x, float y, float z)
{
	return "v " + std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(z) + "\n";
}

std::string Exporter::SetFace(int x, int y, int z)
{
	return "f " + std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(z) + "\n";
}
