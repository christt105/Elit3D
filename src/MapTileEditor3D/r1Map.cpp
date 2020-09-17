#include "r1Map.h"

#include "FileSystem.h"
#include "ExternalTools/JSON/json.hpp"

#include "OpenGLHelper.h"

#include "ExternalTools/DevIL/il.h"
#include "ExternalTools/DevIL/ilut.h"

#include "Profiler.h"

#include "ExternalTools/mmgr/mmgr.h"

r1Map::r1Map(const uint64_t& _uid) : Resource(Resource::Type::Map, _uid)
{
}

r1Map::~r1Map()
{
	Unload();
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
				lay["data"].push_back((*l)->tile_data[i]);
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

		layer->tile_data = new unsigned char[size.x * size.y * 3];
		int i = 0;
		for (auto it = (*l)["data"].begin();
			it != (*l)["data"].end();
			++it, ++i) {
			layer->tile_data[i] = *it;
		}

		oglh::GenTextureData(layer->id_tex, true, true, size.x, size.y, layer->tile_data);

		layers.push_back(layer);
	}
}

void r1Map::Unload()
{
	for (auto l : layers) {
		delete l;
	}
	layers.clear();
}

void r1Map::Resize(int width, int height)
{
	PROFILE_AND_LOG("Map Resize");

	unsigned char* new_data = new unsigned char[width * height * 3];
	{
		PROFILE_SECTION("Set new data");
		for (int i = 0; i < width * height * 3; i += 3) {
			new_data[i] = 0;
			new_data[i + 1] = 255;
			new_data[i + 2] = 0;
		}
	}

	unsigned char* old_data = layers[0]->tile_data;
	{
		PROFILE_SECTION("Copy data");
		for (int i = 0; i < size.x * size.y; ++i) {
			int2 colrow = int2(i % size.x, (int)(i / size.x));
			int new_index = (colrow.x + width * colrow.y) * 3;
			if (new_index < width * height * 3) {
				int old_index = (colrow.x + size.x * colrow.y) * 3;
				for (int j = 0; j < 3; ++j) {
					new_data[new_index + j] = old_data[old_index + j];
				}
			}
		}
	}

	{
		PROFILE_SECTION("Gen Texture");
		layers[0]->tile_data = new_data;
		delete[] old_data;
		layers[0]->size = { width, height };
		size = layers[0]->size;

		oglh::DeleteTexture(layers[0]->id_tex);
		oglh::GenTextureData(layers[0]->id_tex, true, true, width, height, new_data); //TODO: research a faster way to do this
	}
}

void r1Map::Edit(int layer, int row, int col, char r, char g, char b)
{
	//cpu
	unsigned char* loc = layers[layer]->tile_data;
	if ((layers[layer]->size.x * row + col) * 3 >= size.x * size.y * 3)
		LOGE("ASKJDKASFHJG");
	loc[(layers[layer]->size.x * row + col) * 3] = r;
	loc[(layers[layer]->size.x * row + col) * 3 + 1] = g;
	loc[(layers[layer]->size.x * row + col) * 3 + 2] = b;

	//gpu
	oglh::BindTexture(layers[layer]->id_tex);

	unsigned char bits[3];
	bits[0] = r;
	bits[1] = g;
	bits[2] = b;
	oglh::TexSubImage2D(col, row, 1, 1, bits);

	oglh::UnBindTexture();
}

void r1Map::CreateNewMap(int width, int height)
{
	nlohmann::json map;

	map["size"] = { width, height };
	map["properties"] = nlohmann::json::object();

	nlohmann::json data = nlohmann::json::object();

	int byte[3] = { 0, 255, 0 }; // TODO: Save with only one number (gid) & compression?

	for (int i = 0; i < width * height; ++i) {
		data["data"].push_back(byte[0]);
		data["data"].push_back(byte[1]);
		data["data"].push_back(byte[2]);
	}

	map["layers"].push_back(data);

	FileSystem::SaveJSONFile("./Assets/Maps/map.scene", map);
}

inline unsigned int r1Map::GetIndexOf2DArray(unsigned int column, unsigned int row, unsigned int width)
{
	return width * row + column;
}

inline unsigned int r1Map::GetIndexOf2DArray(const int2& colrow, unsigned int width)
{
	return width * colrow.y + colrow.x;
}

inline int2 r1Map::GetColRowOf2DArray(unsigned int index, unsigned int width)
{
	return int2(index % width, (int)(index / width));
}
