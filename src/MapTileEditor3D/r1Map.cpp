#include "r1Map.h"

#include "FileSystem.h"
#include "ExternalTools/JSON/json.hpp"

#include <GL/glew.h>

#include "ExternalTools/DevIL/il.h"
#include "ExternalTools/DevIL/ilut.h"

r1Map::r1Map(const uint64_t& _uid) : Resource(Resource::Type::Map, _uid)
{
}

r1Map::~r1Map()
{
	for (auto l : layers) {
		delete l;
	}
}

void r1Map::Save()
{
	if (references > 0) {
		nlohmann::json file;

		file["properties"] = nlohmann::json::object();
		file["size"] = { size.x, size.y };

		for (auto l = layers.begin(); l != layers.end(); ++l) {
			nlohmann::json lay = nlohmann::json::object();
			//lay["id"] = l - layers.begin();
			for (int i = 0; i < size.x * size.y * 3; ++i) {
				lay["data"].push_back((*l)->data[i]);
			}
			file["layers"].push_back(lay);
		}

		FileSystem::SaveJSONFile(assets_path.c_str(), file);
	}
	else {
		//attach
	}
}

void r1Map::Load()
{
	auto file = FileSystem::OpenJSONFile(assets_path.c_str()); // TODO: OWN FILEFORMAT

	size.x = file["size"][0];
	size.y = file["size"][1];

	for (auto l = file["layers"].begin(); l != file["layers"].end(); ++l) {
		Layer* layer = new Layer();
		layer->size = size;

		layer->data = new unsigned char[size.x * size.y * 3];
		int i = 0;
		for (auto it = (*l)["data"].begin();
			it != (*l)["data"].end();
			++it, ++i) {
			layer->data[i] = *it;
		}

		glGenTextures(1, &layer->id_tex);
		glBindTexture(GL_TEXTURE_2D, layer->id_tex);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, layer->data);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glGenerateMipmap(GL_TEXTURE_2D);

		layers.push_back(layer);
	}
}

void r1Map::Edit(int layer, int row, int col, char r, char g, char b)
{
	//cpu
	unsigned char* loc = layers[layer]->data;

	loc[(layers[layer]->size.x * row + col) * 3] = r;
	loc[(layers[layer]->size.x * row + col) * 3 + 1] = g;
	loc[(layers[layer]->size.x * row + col) * 3 + 2] = b;

	//gpu
	glBindTexture(GL_TEXTURE_2D, layers[layer]->id_tex); //TODO: save data in an array in order to save it later

	unsigned char bits[3];
	bits[0] = r;
	bits[1] = g;
	bits[2] = b;
	glTexSubImage2D(GL_TEXTURE_2D, 0, col, row, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, bits);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void r1Map::CreateNewMap(int width, int height)
{
	nlohmann::json map;

	map["size"] = { width, height };
	map["properties"] = nlohmann::json::object();

	nlohmann::json data = nlohmann::json::object();

	int byte[3] = { 0, 255, 0 };

	for (int i = 0; i < width * height; ++i) {
		data["data"].push_back(byte[0]);
		data["data"].push_back(byte[1]);
		data["data"].push_back(byte[2]);
	}

	map["layers"].push_back(data);

	FileSystem::SaveJSONFile("./Assets/Maps/map.scene", map);
}
