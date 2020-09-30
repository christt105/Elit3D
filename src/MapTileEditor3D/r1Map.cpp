#include "r1Map.h"

#include "FileSystem.h"

#include "OpenGLHelper.h"

#include "Application.h"
#include "m1GUI.h"
#include "p1Tileset.h"

#include "MapLayer.h"

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

void r1Map::Save(const uint64_t& tileset)
{
	if (references > 0) {
		nlohmann::json file;
		
		file["properties"] = nlohmann::json::object();
		file["size"] = { size.x, size.y };
		file["tileset"] = tileset;

		for (auto l = layers.begin(); l != layers.end(); ++l) {
			nlohmann::json lay = nlohmann::json::object();
			for (int i = 0; i < size.x * size.y * 3; ++i) {
				lay["data"].push_back((*l)->tile_data[i]);
			}

			for (auto p = (*l)->properties.begin(); p != (*l)->properties.end(); ++p) {
				nlohmann::json prop = nlohmann::json::object();
				prop["name"] = (*p).first;
				prop["type"] = (*p).second->type;
				switch ((*p).second->type)
				{
				case TypeVar::Type::Int:
					prop["value"] = static_cast<iTypeVar*>((*p).second)->value;
					break;
				case TypeVar::Type::String:
					prop["value"] = static_cast<sTypeVar*>((*p).second)->value;
					break;
				case TypeVar::Type::Float:
					prop["value"] = static_cast<fTypeVar*>((*p).second)->value;
					break;
				case TypeVar::Type::Bool:
					prop["value"] = static_cast<bTypeVar*>((*p).second)->value;
					break;
				default:
					break;
				}

				lay["properties"].push_back(prop);
			}

			lay["name"] = (*l)->GetName();
			lay["height"] = (*l)->height;
			lay["opacity"] = (*l)->opacity;
			lay["visible"] = (*l)->visible;
			lay["locked"] = (*l)->locked;
			lay["displacement"] = { (*l)->displacement[0], (*l)->displacement[1] };

			file["layers"].push_back(lay);
		}

		FileSystem::SaveJSONFile(assets_path.c_str(), file);
	}
	else {
		//attach
	}
}

void r1Map::SaveInImage()
{
	if (!FileSystem::Exists("Export/"))
		FileSystem::CreateFolder("Export/");
	if (!FileSystem::Exists("Export/Debug/"))
		FileSystem::CreateFolder("Export/Debug/");

	ilEnable(IL_FILE_OVERWRITE);
	ilutGLSaveImage((char*)"Export/Debug/MAP_IMAGE_LAYER0.png", layers[0]->id_tex);
}

void r1Map::Load()
{
	auto file = FileSystem::OpenJSONFile(assets_path.c_str()); // TODO: OWN FILEFORMAT

	size.x = file["size"][0];
	size.y = file["size"][1];

	App->gui->tileset->SelectTileset(file.value("tileset", 0ULL));

	LoadLayers(file);
}

void r1Map::LoadLayers(nlohmann::json& file)
{
	for (auto l = file["layers"].begin(); l != file["layers"].end(); ++l) {
		Layer* layer = new Layer();
		layer->SetName((*l).value("name", "Layer").c_str());
		layer->height = (*l).value("height", 0.f);
		layer->opacity = (*l).value("opacity", 1.f);
		layer->visible = (*l).value("visible", true);
		layer->locked = (*l).value("locked", false);
		if ((*l).find("displacement") != (*l).end()) {
			layer->displacement[0] = (*l)["displacement"][0];
			layer->displacement[1] = (*l)["displacement"][1];
		}

		LoadProperties(l, layer);

		layer->tile_data = new unsigned char[size.x * size.y * 3];
		int i = 0;
		for (auto it = (*l)["data"].begin();
			it != (*l)["data"].end();
			++it, ++i) {
			layer->tile_data[i] = *it;
		}
		glEnable(GL_TEXTURE_2D);
		oglh::GenTextureData(layer->id_tex, false, true, size.x, size.y, layer->tile_data);
		oglh::UnBindTexture();


		layers.push_back(layer);
	}
}

void r1Map::LoadProperties(const nlohmann::detail::iter_impl<nlohmann::json>& l, Layer* layer)
{
	for (auto p = (*l)["properties"].begin(); p != (*l)["properties"].end(); ++p) {
		switch ((TypeVar::Type)(*p).value("type", 0))
		{
		case TypeVar::Type::Int:
			layer->properties[(*p).value("name", "UNKNOWN")] = new iTypeVar((*p).value("value", 0));
			break;
		case TypeVar::Type::Float:
			layer->properties[(*p).value("name", "UNKNOWN")] = new fTypeVar((*p).value("value", 0.f));
			break;
		case TypeVar::Type::Bool:
			layer->properties[(*p).value("name", "UNKNOWN")] = new bTypeVar((*p).value("value", false));
			break;
		case TypeVar::Type::String:
			layer->properties[(*p).value("name", "UNKNOWN")] = new sTypeVar((*p).value("value", std::string()));
			break;
		default:
			break;
		}
	}
}

void r1Map::Unload()
{
	for (auto l : layers) {
		delete l;
	}
	layers.clear();

	for (auto p : properties) {
		delete p.second;
	}
	properties.clear();
}

void r1Map::Resize(int width, int height)
{
	PROFILE_AND_LOG("Map Resize");

	unsigned char* empty = new unsigned char[width * height * 3];
	for (int i = 0; i < width * height * 3; i += 3) {
		empty[i] = 0;
		empty[i + 1] = 255;
		empty[i + 2] = 0;
	}


	for (auto l = layers.begin(); l != layers.end(); ++l) {
		unsigned char* new_data = new unsigned char[width * height * 3];
		memcpy(new_data, empty, width * height * 3);

		{
			PROFILE_SECTION("Copy data");
			for (int i = 0; i < size.x * size.y; ++i) {
				int2 colrow = int2(i % size.x, (i / size.x));
				int new_index = (colrow.x + width * colrow.y) * 3;
				if (new_index < width * height * 3) {
					int old_index = (colrow.x + size.x * colrow.y) * 3;
					for (int j = 0; j < 3; ++j) {
						new_data[new_index + j] = (*l)->tile_data[old_index + j];
					}
				}
			}
		}

		{
			PROFILE_SECTION("Gen Texture");
			delete[](*l)->tile_data;
			(*l)->tile_data = new_data;

			oglh::DeleteTexture((*l)->id_tex);
			oglh::GenTextureData((*l)->id_tex, true, true, width, height, (*l)->tile_data); //TODO: research a faster way to do this
		}
	}
	size = { width, height };
	delete[] empty;
}

void r1Map::Edit(int layer, int row, int col, char r, char g, char b)
{
	//cpu
	unsigned char* loc = layers[layer]->tile_data;
	loc[(size.x * row + col) * 3] = r;
	loc[(size.x * row + col) * 3 + 1] = g;
	loc[(size.x * row + col) * 3 + 2] = b;

	//gpu
	oglh::BindTexture(layers[layer]->id_tex);

	unsigned char bits[3];
	bits[0] = r;
	bits[1] = g;
	bits[2] = b;
	oglh::TexSubImage2D(col, row, 1, 1, bits);

	oglh::UnBindTexture();
}

void r1Map::CreateNewMap(int width, int height, const char* file)
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

	FileSystem::SaveJSONFile(file, map);
}
