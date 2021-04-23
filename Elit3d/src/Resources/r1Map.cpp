#include "Resources/r1Map.h"

#include <queue>

#include "Tools/FileSystem.h"

#include "Tools/OpenGL/OpenGLHelper.h"

#include "Core/Application.h"
#include "Modules/m1GUI.h"
#include "Panels/p1Tileset.h"
#include "Panels/p1Layers.h"
#include "Modules/m1Resources.h"
#include "Resources/r1Tileset.h"

#include "Tools/Map/MapLayer.h"
#include "Tools/Map/MapLayerTile.h"
#include "Objects/Object.h"

#include "ExternalTools/DevIL/include/IL/il.h"
#include "ExternalTools/DevIL/include/IL/ilut.h"

#include "ExternalTools/Assimp/include/scene.h"
#include "ExternalTools/Assimp/include/Exporter.hpp"

#include "Tools/System/Logger.h"

#include "Tools/System/Profiler.h"

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
		file["version"] = App->GetVersion();

		properties.SaveProperties(file["properties"]);

		for (auto l = layers.begin(); l != layers.end(); ++l) {
			auto layer = nlohmann::json();
			(*l)->Parse(layer, MapLayer::DataTypeExport::BASE64_ZLIB);
			file["layers"].push_back(layer);
		}

		FileSystem::SaveJSONFile(path.c_str(), file);
	}
	else {
		//TODO: attach
	}
}

void r1Map::Export(const uint64_t& tileset, MapLayer::DataTypeExport d, m1MapEditor::MapTypeExport t)
{
	if (references > 0) {
		switch (t) //TODO: Export UID objects table
		{
		case m1MapEditor::MapTypeExport::XML:
			ExportXML(tileset, d);
			break;
		case m1MapEditor::MapTypeExport::JSON:
			ExportJSON(tileset, d);
			break;
		case m1MapEditor::MapTypeExport::OBJ:
			ExportOBJ();
			break;
		}
	}
}

void r1Map::ExportJSON(const uint64_t& tileset, MapLayer::DataTypeExport d)
{
	nlohmann::json file;

	file["size"] = { size.x, size.y };
	file["version"] = App->GetVersion();
	
	if (auto image = (r1Tileset*)App->resources->Get(tileset); image != nullptr)
		file["tileset"] = image->path; //TODO: object tileset. All info of tileset binded on map file exported

	properties.SaveProperties(file["properties"]);

	for (auto l = layers.begin(); l != layers.end(); ++l) {
		nlohmann::json lay = nlohmann::json::object();
		(*l)->Parse(lay, d);

		file["layers"].push_back(lay);
	}

	FileSystem::SaveJSONFile("Export/Test.json", file);
}

void r1Map::ExportXML(const uint64_t& tileset, MapLayer::DataTypeExport d)
{
	pugi::xml_document doc;
	pugi::xml_node map = doc.append_child("map");

	map.append_attribute("width").set_value(size.x);
	map.append_attribute("height").set_value(size.y);
	map.append_attribute("version").set_value(App->GetVersion());

	if (auto tile = (r1Tileset*)App->resources->Get(tileset); tile != nullptr) {
		tile->Parse(map.append_child("tileset"));
	}

	properties.SaveProperties(map.append_child("properties"));

	for (auto l = layers.begin(); l != layers.end(); ++l) {
		(*l)->Parse(map.append_child("layer"), d);
	}

	doc.save_file("Export/Test.xml");
}

void r1Map::ExportOBJ() const
{
	std::unique_ptr<aiScene> scene(new aiScene());

	scene->mRootNode = new aiNode();

	std::vector<aiMesh*> meshes;

	aiNode** children = new aiNode*[layers.size()];
	int i = 0;
	for (auto& l : layers) {
		children[i++] = l->Parse(meshes);
	}
	scene->mRootNode->addChildren(layers.size(), children);

	scene->mNumMeshes = meshes.size();
	scene->mMeshes = new aiMesh * [scene->mNumMeshes];
	i = 0;
	for (auto& m : meshes) {
		scene->mMeshes[i++] = m;
	}
	scene->mNumMaterials = 1;
	scene->mMaterials = new aiMaterial * [1];
	scene->mMaterials[0] = new aiMaterial();
	//scene->mRootNode->mNumMeshes = 0;
	//scene->mRootNode->mMeshes = new unsigned int[0];
	//scene->mRootNode->mMeshes[0] = 0;

	Assimp::Exporter exporter;
	if (exporter.Export(scene.get(), "obj", "Export/test.obj") != AI_SUCCESS)
		LOGE(exporter.GetErrorString())

		scene.release();
}

void r1Map::SaveInImage()
{
	if (!FileSystem::Exists("Export/"))
		FileSystem::CreateFolder("Export/");
	if (!FileSystem::Exists("Export/Debug/"))
		FileSystem::CreateFolder("Export/Debug/");

	ilEnable(IL_FILE_OVERWRITE);
	for (auto i = layers.begin(); i != layers.end(); ++i) {
		if ((*i)->type != MapLayer::Type::TILE)
			continue;
		ilutGLSaveImage((char*)("Export/Debug/MAP_IMAGE_LAYER_" + name + std::to_string(i - layers.begin()) + ".png").c_str(), ((MapLayerTile*)layers[i - layers.begin()])->id_tex); // TODO: crash on application quit
	}
}

void r1Map::Load()
{
	auto file = FileSystem::OpenJSONFile(path.c_str()); // TODO: OWN FILEFORMAT

	size.x = file["size"][0];
	size.y = file["size"][1];

	App->gui->tileset->SelectTileset(file.value("tileset", 0ULL));

	properties.LoadProperties(file["properties"]);

	for (auto l = file["layers"].begin(); l != file["layers"].end(); ++l) {
		MapLayer* layer = App->map_editor->AddLayer((*l).value("type", MapLayer::Type::TILE));
		if (layer != nullptr)
			layer->Unparse(*l);
	}

	std::sort(layers.begin(), layers.end(), MapLayer::HeightOrder);
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
		(*l)->Resize(size, { width, height });
	}
	size = { width, height };
}

void r1Map::Edit(MapLayerTile* layer, int row, int col, int brushSize, p1Tools::Tools tool, p1Tools::Shape shape, TILE_DATA_TYPE id, unsigned char g, unsigned char b)
{
	oglh::BindTexture(layer->id_tex);
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
						if (index >= 0 && layer->data[index] != id && index < size.x * size.y) {
							layer->data[index] = id;
							oglh::TexSubImage2D(i, j, 1, 1, bits); //TODO: TexSubImage2d of all rectangle
						}
					}
				}
			}
			break;
		case p1Tools::Shape::CIRCLE: {
			float r = (float)brushSize * 0.5f;
			float r2 = r * r;
			for (int i = (brushSize % 2 == 0) ? col - r + 1 : col - r; i < col + r + 1; ++i) {
				for (int j = (brushSize % 2 == 0) ? row - r + 1 : row - r; j < row + r + 1; ++j) {
					if (i >= 0 && j >= 0 && i < size.x && j < size.y) {
						float check = (brushSize % 2 == 0) ?
							((float)i - col - 0.5f) * ((float)i - col - 0.5f) + ((float)j - row - 0.5f) * ((float)j - row - 0.5f)
							:
							((float)i - col) * ((float)i - col) + ((float)j - row) * ((float)j - row);
						if (check <= r2) {
							int index = size.x * j + i;
							if (index >= 0 && layer->data[index] != id && index < size.x * size.y) {
								layer->data[index] = id;
								oglh::TexSubImage2D(i, j, 1, 1, bits); //TODO: TexSubImage2d of all rectangle
							}
						}
					}
				}
			}
			break; }
		default:
			break;
		}
		break;
	case p1Tools::Tools::BUCKET: {
		PROFILE_AND_LOG("Bucket");
		bool* visited = new bool[size.x * size.y];
		memset(visited, false, size.x * size.y);
		std::queue<std::pair<int, int>> queue;
		visited[size.x * row + col] = true;
		queue.push({col, row});
		int baseIndex = layer->data[size.x * row + col];

		while (!queue.empty()) {
			auto t = queue.front(); queue.pop();
			layer->data[size.x * t.second + t.first] = id;
			oglh::TexSubImage2D(t.first, t.second, 1, 1, bits);
			
			if (t.first + 1 >= 0 && t.second >= 0 && t.first + 1 < size.x && t.second < size.y &&
				!visited[size.x * t.second + t.first + 1] &&
				layer->data[size.x * t.second + t.first + 1] == baseIndex) {
				visited[size.x * t.second + t.first + 1] = true;
				queue.push({ t.first + 1, t.second });
			}
			if (t.first - 1 >= 0 && t.second >= 0 && t.first - 1 < size.x && t.second < size.y &&
				!visited[size.x * t.second + t.first - 1] &&
				layer->data[size.x * t.second + t.first - 1] == baseIndex) {
				visited[size.x * t.second + t.first - 1] = true;
				queue.push({ t.first - 1, t.second });
			}
			if (t.first >= 0 && t.second + 1 >= 0 && t.first < size.x && t.second + 1 < size.y &&
				!visited[size.x * (t.second + 1) + t.first] &&
				layer->data[size.x * (t.second + 1) + t.first] == baseIndex) {
				visited[size.x * (t.second + 1) + t.first] = true;
				queue.push({ t.first, t.second + 1 });
			}
			if (t.first >= 0 && t.second - 1 >= 0 && t.first < size.x && t.second - 1 < size.y &&
				!visited[size.x * (t.second - 1) + t.first] &&
				layer->data[size.x * (t.second - 1) + t.first] == baseIndex) {
				visited[size.x * (t.second - 1) + t.first] = true;
				queue.push({ t.first, t.second - 1 });
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

int2 r1Map::GetSize() const
{
	return size;
}

void r1Map::GetSize(int& x, int& y) const
{
	x = size.x;
	y = size.y;
}

void r1Map::GetSize(int2& size) const
{
	size.x = this->size.x;
	size.y = this->size.y;
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
