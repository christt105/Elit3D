#include "r1Map.h"

#include <queue>

#include "FileSystem.h"

#include "OpenGLHelper.h"

#include "Application.h"
#include "m1GUI.h"
#include "p1Tileset.h"
#include "p1Layers.h"
#include "m1Resources.h"
#include "r1Tileset.h"

#include "MapLayer.h"

#include "ExternalTools/DevIL/include/IL/il.h"
#include "ExternalTools/DevIL/include/IL/ilut.h"

#include "Profiler.h"

#include "ExternalTools/pugixml/pugixml.hpp"

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
		
		file["size"] = { size.x, size.y };
		file["tileset"] = tileset;

		properties.SaveProperties(file["properties"]);

		for (auto l = layers.begin(); l != layers.end(); ++l) {
			nlohmann::json lay = nlohmann::json::object();
			lay["encoding"] = "base64-zlib";
			lay["data"] = (*l)->Parse(size.x, size.y, Layer::DataTypeExport::BASE64_ZLIB);

			(*l)->properties.SaveProperties(lay["properties"]);

			lay["name"] = (*l)->GetName();
			lay["height"] = (*l)->height;
			lay["opacity"] = (*l)->opacity;
			lay["visible"] = (*l)->visible;
			lay["locked"] = (*l)->locked;
			lay["displacement"] = { (*l)->displacement[0], (*l)->displacement[1] };

			file["layers"].push_back(lay);
		}

		FileSystem::SaveJSONFile(path.c_str(), file);
	}
	else {
		//TODO: attach
	}
}

void r1Map::Export(const uint64_t& tileset, Layer::DataTypeExport d, m1MapEditor::MapTypeExport t)
{
	if (references > 0) {
		switch (t)
		{
		case m1MapEditor::MapTypeExport::XML: {
			pugi::xml_document doc;
			pugi::xml_node map = doc.append_child("map");

			map.append_attribute("width").set_value(size.x);
			map.append_attribute("height").set_value(size.y);

			pugi::xml_node ntileset = map.append_child("tileset");
			auto tile = (r1Tileset*)App->resources->Get(tileset);
			if (tile != nullptr) {
				ntileset.append_attribute("name").set_value(tile->name.c_str());
				ntileset.append_attribute("tilewidth").set_value(tile->GetWidth());
				ntileset.append_attribute("tileheight").set_value(tile->GetHeight());
				ntileset.append_attribute("spacing").set_value(tile->GetSpacing());
				ntileset.append_attribute("margin").set_value(tile->GetMargin());
				ntileset.append_attribute("ntiles").set_value(tile->GetNTiles());
				ntileset.append_attribute("columns").set_value(tile->GetColumns());

				ntileset.append_child("image").append_attribute("src").set_value(tile->path.c_str());
			}

			pugi::xml_node xmlproperties = map.append_child("properties");
			properties.SaveProperties(xmlproperties);

			for (auto l = layers.begin(); l != layers.end(); ++l) {
				pugi::xml_node layer = map.append_child("layer");

				xmlproperties = layer.append_child("properties");
				(*l)->properties.SaveProperties(xmlproperties);

				layer.append_attribute("name").set_value((*l)->GetName());
				layer.append_attribute("visible").set_value((*l)->visible);
				layer.append_attribute("locked").set_value((*l)->locked);
				layer.append_attribute("height").set_value((*l)->height);
				layer.append_attribute("opacity").set_value((*l)->opacity);
				layer.append_attribute("displacementx").set_value((*l)->displacement[0]);
				layer.append_attribute("displacementy").set_value((*l)->displacement[1]);

				auto data = layer.append_child("data");
				auto encoding = data.append_attribute("encoding");
				switch (d)
				{
				case Layer::DataTypeExport::CSV:
				case Layer::DataTypeExport::CSV_NO_NEWLINE:
					encoding.set_value("csv");
					break;
				case Layer::DataTypeExport::BASE64_NO_COMPRESSION:
					encoding.set_value("base64");
					break;
				case Layer::DataTypeExport::BASE64_ZLIB:
					encoding.set_value("base64-zlib");
					break;
				default:
					break;
				}
				data.append_child(pugi::node_pcdata).set_value((*l)->Parse(size.x, size.y, d).c_str());
			}

			doc.save_file("../../Export/Test.xml");
			break;
		}
		case m1MapEditor::MapTypeExport::JSON:
		{
			nlohmann::json file;

			file["size"] = { size.x, size.y };
			auto image = App->resources->Get(tileset);
			if (image != nullptr)
				file["tileset"] = image->path; //TODO: object tileset. All info of tileset binded on map file exported

			properties.SaveProperties(file["properties"]);

			for (auto l = layers.begin(); l != layers.end(); ++l) {
				nlohmann::json lay = nlohmann::json::object();

				switch (d)
				{
				case Layer::DataTypeExport::CSV:
				case Layer::DataTypeExport::CSV_NO_NEWLINE:
					lay["encoding"] = "csv";
					lay["data"] = (*l)->Parse(size.x, size.y);
					break;
				case Layer::DataTypeExport::BASE64_NO_COMPRESSION:
					lay["encoding"] = "base64";
					lay["data"] = (*l)->Parse(size.x, size.y, d);
					break;
				case Layer::DataTypeExport::BASE64_ZLIB:
					lay["encoding"] = "base64-zlib";
					lay["data"] = (*l)->Parse(size.x, size.y, d);
					break;
				default:
					break;
				}

				(*l)->properties.SaveProperties(lay["properties"]);

				lay["name"] = (*l)->GetName();
				lay["height"] = (*l)->height;
				lay["opacity"] = (*l)->opacity;
				lay["visible"] = (*l)->visible;
				lay["locked"] = (*l)->locked;
				lay["displacement"] = { (*l)->displacement[0], (*l)->displacement[1] };

				file["layers"].push_back(lay);
			}

			FileSystem::SaveJSONFile("../../Export/Test.json", file);
			break;
		}
		}
	}
}

void r1Map::SaveInImage()
{
	if (!FileSystem::Exists("../../Export/"))
		FileSystem::CreateFolder("../../Export/");
	if (!FileSystem::Exists("../../Export/Debug/"))
		FileSystem::CreateFolder("../../Export/Debug/");

	ilEnable(IL_FILE_OVERWRITE);
	for (auto i = layers.begin(); i != layers.end(); ++i) {
		ilutGLSaveImage((char*)("../../Export/Debug/MAP_IMAGE_LAYER_" + name + std::to_string(i - layers.begin()) + ".png").c_str(), layers[i - layers.begin()]->id_tex); // TODO: crash on application quit
	}
}

void r1Map::Load()
{
	auto file = FileSystem::OpenJSONFile(path.c_str()); // TODO: OWN FILEFORMAT

	size.x = file["size"][0];
	size.y = file["size"][1];

	App->gui->tileset->SelectTileset(file.value("tileset", 0ULL));

	properties.LoadProperties(file["properties"]);

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

		layer->properties.LoadProperties((*l)["properties"]);


		layer->tile_data = new TILE_DATA_TYPE[size.x * size.y];
		layer->Unparse(size.x, size.y, (*l).value("data", "0")/*TODO: unparse type (csv, base64, zlib...)*/);

		unsigned char* tex_data = new unsigned char[size.x * size.y * 3];
		memset(tex_data, 254, sizeof(unsigned char) * size.x * size.y * 3);

		for (int i = 0; i < size.x * size.y; ++i) {
			if (layer->tile_data[i] != 0) {
				tex_data[i * 3] = (unsigned char)(layer->tile_data[i] / UCHAR_MAX);
				tex_data[i * 3 + 1] = 0;
				tex_data[i * 3 + 2] = (unsigned char)(layer->tile_data[i] % UCHAR_MAX);
			}
		}

		glEnable(GL_TEXTURE_2D);
		oglh::GenTextureData(layer->id_tex, oglh::Wrap::Repeat, oglh::Filter::Nearest, size.x, size.y, tex_data);
		oglh::UnBindTexture();
		delete[] tex_data;

		layers.push_back(layer);
	}

	std::sort(layers.begin(), layers.end(), Layer::HeightOrder);
	App->gui->layers->SetSelected(0);
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

	for (auto l = layers.begin(); l != layers.end(); ++l) {
		TILE_DATA_TYPE* new_data = new TILE_DATA_TYPE[width * height];
		memset(new_data, 0, sizeof(TILE_DATA_TYPE) * width * height);

		{
			PROFILE_SECTION("Copy data");
			for (int i = 0; i < size.x * size.y; ++i) {
				int2 colrow = int2(i % size.x, (i / size.x));
				if (colrow.x < width && colrow.y < height) {
					int new_index = (colrow.x + width * colrow.y);
					int old_index = (colrow.x + size.x * colrow.y);
					new_data[new_index] = (*l)->tile_data[old_index];
				}
			}
		}

		{
			PROFILE_SECTION("Gen Texture");
			delete[](*l)->tile_data;
			(*l)->tile_data = new_data;

			unsigned char* tex = new unsigned char[width * height * 3];
			memset(tex, 0, sizeof(unsigned char) * width * height * 3);
			int tileset_width = App->gui->tileset->GetTilesetSize().x;
			for (auto i = 0; i < width * height; ++i) {
				if (new_data[i] != 0) {
					tex[i * 3] = (unsigned char)(new_data[i] / UCHAR_MAX);
					tex[i * 3 + 1] = 0;
					tex[i * 3 + 2] = (unsigned char)(new_data[i] % UCHAR_MAX);
				}
			}

			oglh::DeleteTexture((*l)->id_tex);
			oglh::GenTextureData((*l)->id_tex, oglh::Wrap::Repeat, oglh::Filter::Nearest, width, height, tex); //TODO: research a faster way to do this
			delete[] tex;
		}
	}
	size = { width, height };
}

void r1Map::Edit(int layer, int row, int col, int brushSize, p1Tools::Tools tool, p1Tools::Shape shape, TILE_DATA_TYPE id, unsigned char g, unsigned char b)
{
	oglh::BindTexture(layers[layer]->id_tex);
	unsigned char bits[3] = { g, 0, b };

	switch (tool)
	{
	case p1Tools::Tools::BRUSH:
	case p1Tools::Tools::ERASER:
		switch (shape)
		{
		case p1Tools::Shape::RECTANGLE:
			for (int i = (brushSize % 2 == 0) ? col - brushSize / 2 + 1 : col - brushSize / 2; i < col + brushSize / 2 + 1; ++i) {
				for (int j = (brushSize % 2 == 0) ? row - brushSize / 2 + 1 : row - brushSize / 2; j < row + brushSize / 2 + 1; ++j) {
					if (i >= 0 && j >= 0 && i < size.x && j < size.y) {
						int index = size.x * j + i;
						if (index >= 0 && layers[layer]->tile_data[index] != id && index < size.x * size.y) {
							layers[layer]->tile_data[index] = id;
							oglh::TexSubImage2D(i, j, 1, 1, bits); //TODO: TexSubImage2d of all rectangle
						}
					}
				}
			}
			break;
		case p1Tools::Shape::CIRCLE:
			break;
		default:
			break;
		}
		break;
	case p1Tools::Tools::BUCKET: {
		PROFILE_AND_LOG("Bucket");
		bool* visited = new bool[size.x * size.y];
		memset(visited, false, size.x * size.y);
		std::queue<Node> queue;
		visited[size.x * row + col] = true;
		queue.push(Node(col,row));
		int baseIndex = layers[layer]->tile_data[size.x * row + col];

		while (!queue.empty()) {
			auto t = queue.front(); queue.pop();
			layers[layer]->tile_data[size.x * t.y + t.x] = id;
			oglh::TexSubImage2D(t.x, t.y, 1, 1, bits);
			
			if (t.x + 1 >= 0 && t.y >= 0 && t.x + 1 < size.x && t.y < size.y &&
				!visited[size.x * t.y + t.x + 1] &&
				layers[layer]->tile_data[size.x * t.y + t.x + 1] == baseIndex) {
				visited[size.x * t.y + t.x + 1] = true;
				queue.push({ t.x + 1, t.y });
			}
			if (t.x - 1 >= 0 && t.y >= 0 && t.x - 1 < size.x && t.y < size.y &&
				!visited[size.x * t.y + t.x - 1] &&
				layers[layer]->tile_data[size.x * t.y + t.x - 1] == baseIndex) {
				visited[size.x * t.y + t.x - 1] = true;
				queue.push({ t.x - 1, t.y });
			}
			if (t.x >= 0 && t.y + 1 >= 0 && t.x < size.x && t.y + 1 < size.y &&
				!visited[size.x * (t.y + 1) + t.x] &&
				layers[layer]->tile_data[size.x * (t.y + 1) + t.x] == baseIndex) {
				visited[size.x * (t.y + 1) + t.x] = true;
				queue.push({ t.x, t.y + 1 });
			}
			if (t.x >= 0 && t.y - 1 >= 0 && t.x < size.x && t.y - 1 < size.y &&
				!visited[size.x * (t.y - 1) + t.x] &&
				layers[layer]->tile_data[size.x * (t.y - 1) + t.x] == baseIndex) {
				visited[size.x * (t.y - 1) + t.x] = true;
				queue.push({ t.x, t.y - 1 });
			}
		}
		delete[] visited;
		break;
	}
	case p1Tools::Tools::EYEDROPPER:
		break;
	case p1Tools::Tools::RECTANGLE:
		break;
	default:
		break;
	}

	oglh::UnBindTexture();
}

void r1Map::CreateNewMap(int width, int height, const char* file)
{
	nlohmann::json map;

	map["size"] = { width, height };
	map["properties"] = nlohmann::json::object();

	nlohmann::json data = nlohmann::json::object();

	std::string tiles;
	for (int i = 0; i < width * height; ++i) {
		tiles += "0";
		if (i != width * height - 1) {
			tiles += ",";
		}
	}
	data["data"] = tiles;

	map["layers"].push_back(data);

	FileSystem::SaveJSONFile(file, map);
}

void r1Map::OnInspector()
{
	if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Text("Width");  ImGui::SameLine(); ImGui::TextColored(ORANGE, std::to_string(size.x).c_str());
		ImGui::Text("Height"); ImGui::SameLine(); ImGui::TextColored(ORANGE, std::to_string(size.y).c_str());
	}
	if (ImGui::CollapsingHeader("Custom Properties", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen)) {
		properties.Display();
	}
}

bool r1Map::CheckBoundaries(const int2& point, int brushSize, p1Tools::Tools tool, p1Tools::Shape shape) const
{
	switch (tool)
	{
	case p1Tools::Tools::BRUSH:
	case p1Tools::Tools::ERASER:
		return point.x + brushSize / 2 >= 0 &&
			((brushSize % 2 != 0) ? point.x - brushSize / 2 : point.x - brushSize / 2 + 1) < size.x &&
			point.y + brushSize / 2 >= 0 &&
			((brushSize % 2 != 0) ? point.y - brushSize / 2 : point.y - brushSize / 2 + 1) < size.y;
	default:
		return point.x >= 0 && point.y >= 0 && point.x < size.x&& point.y < size.y;
	}
	return false;
}
