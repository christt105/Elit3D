#include "Resources/r1Map.h"

#include <queue>

#include "Tools/FileSystem.h"

#include "Tools/OpenGL/OpenGLHelper.h"

#include "Core/Application.h"
#include "Modules/m1GUI.h"
#include "Panels/p1Tileset.h"
#include "Panels/p1Objects.h"
#include "Panels/p1Layers.h"
#include "Modules/m1Resources.h"
#include "Resources/r1Tileset.h"
#include "Resources/r1Tileset3d.h"
#include "Resources/r1Object.h"
#include "Resources/r1Texture.h"
#include "Resources/r1Model.h"
#include "Resources/r1Mesh.h"

#include "ExternalTools/MathGeoLib/include/Math/float2.h"
#include "Tools/Map/MapLayer.h"
#include "Tools/Map/MapLayerTile.h"
#include "Tools/Map/MapLayerTerrain.h"
#include "Objects/Object.h"
#include "Objects/Components/c1Mesh.h"
#include "Objects/Components/c1Material.h"

#include "ExternalTools/DevIL/include/IL/il.h"
#include "ExternalTools/DevIL/include/IL/ilut.h"

#include "ExternalTools/Assimp/include/scene.h"
#include "ExternalTools/Assimp/include/Exporter.hpp"
#include "Tools/Importers/Exporter.h"

#include "ExternalTools/lodepng/lodepng.h"

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
			(*l)->Parse(layer, MapLayer::DataTypeExport::BASE64_ZLIB, false);
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

	if (auto tile = (r1Tileset*)App->resources->Get(tileset); tile != nullptr)
		tile->Parse(file["tileset"]);

	App->gui->objects->tileset->Parse(file["tileset3D"]);

	/*std::vector<uint64_t> usedObjects;
	for (auto& l : layers) {
		if (l->type != MapLayer::Type::OBJECT)
			continue;

		MapLayerTerrain* lay = (MapLayerTerrain*)l;
		lay->root->GetUsedObjects(usedObjects);
	}

	for (auto o : usedObjects) {
		if (auto res = App->resources->Get(o); res != nullptr) {
			file["objects"].push_back(res->path);
		}
	}*/

	properties.SaveProperties(file["properties"]);

	for (auto l = layers.rbegin(); l != layers.rend(); ++l) {
		nlohmann::json lay = nlohmann::json::object();
		(*l)->Parse(lay, d, true);

		/*if ((*l)->type == MapLayer::Type::OBJECT)
			((MapLayerTerrain*)(*l))->AddObjects(lay["objects"], usedObjects);*/

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

	App->gui->objects->tileset->Parse(map.append_child("tileset3D"));

	/*std::vector<uint64_t> usedObjects;
	for (auto& l : layers) {
		if (l->type != MapLayer::Type::OBJECT)
			continue;

		MapLayerTerrain* lay = (MapLayerTerrain*)l;
		lay->root->GetUsedObjects(usedObjects);
	}*/

	/*auto objects = map.append_child("objects");
	for (auto o : usedObjects) {
		if (auto res = App->resources->Get(o); res != nullptr) {
			objects.append_child("objects").append_attribute("path").set_value(res->path.c_str());
		}
	}*/

	properties.SaveProperties(map.append_child("properties"));

	auto maplayers = map.append_child("layers");
	for (auto l = layers.rbegin(); l != layers.rend(); ++l) {
		auto xlayer = maplayers.append_child("layer");
		(*l)->Parse(xlayer, d, true);

		/*if ((*l)->type == MapLayer::Type::OBJECT)
			((MapLayerTerrain*)(*l))->AddObjects(xlayer.append_child("objects"), usedObjects);*/
	}

	doc.save_file("Export/Test.xml");
}

void r1Map::ExportOBJ() const
{
	std::string file;
	std::string mtl;

	file += "# Map made with Elit3D version " + std::string(App->GetVersion()) + "\n";

	file += "mtllib test.mtl\n";

	file += "# Planes\n";

	int nvertex = 1;
	int ntexture = 0;
	int nnormal = 0;
	auto tileset = App->gui->tileset;
	int l_index = 0;

	mtl += "newmtl tileset\n";
	const r1Tileset* rtileset = (r1Tileset*)App->resources->Get(tileset->GetTileset());
	assert(rtileset != nullptr);
	const r1Texture* tex = (r1Texture*)App->resources->Get(rtileset->GetTextureUID());
	std::string full_name = tex->name + "." + tex->extension;
	mtl += "map_Kd " + full_name + "\n\n";
	FileSystem::CopyTo(tex->path.c_str(), ("Export/" + full_name).c_str());

	for (auto& l : layers) {
		if (l->type != MapLayer::Type::TILE)
			continue;

		std::map<unsigned int, std::vector<unsigned int>> tiles; // <tile_id, index_position>
		for (unsigned int i = 0U; i < size.x * size.y; ++i) {
			if (l->data[i] == 0U)
				continue;
			tiles[l->data[i]].push_back(i);
		}
		
		for (auto t = tiles.begin(); t != tiles.end(); ++t) {
			unsigned int tile_id = (*t).first;
			std::string verttex;
			std::string face;
			file += "o layer_" + std::to_string(l_index) + "_tile_" + std::to_string(tile_id) + "\n";
			const int2 tilesetSizeOnTiles = tileset->GetTilesetSize();
			const int2 tileSize = tileset->GetTileSize();
			const int2 tilesetSize = tilesetSizeOnTiles * tileSize;
			const int2 tile(tile_id % tilesetSizeOnTiles.x, tile_id / tilesetSizeOnTiles.x);
			const float2 tileSizeTexCoord = float2((float)tileSize.x / (float)tilesetSize.x, (float)tileSize.y / (float)tilesetSize.y);
			
			int2 pixelCoord = tile * tileSize;
			float2 texCoord = float2((float)pixelCoord.x / (float)tilesetSize.x, (float)pixelCoord.y / (float)tilesetSize.y);

			face += "usemtl tileset\n";

			for (auto& v : (*t).second) {
				int2 coord(v / size.x, v % size.x);

				file += Exporter::SetVertex(coord.x,	 l->height, coord.y);
				file += Exporter::SetVertex(coord.x + 1, l->height, coord.y);
				file += Exporter::SetVertex(coord.x + 1, l->height, coord.y + 1);
				file += Exporter::SetVertex(coord.x,	 l->height, coord.y + 1);

				verttex += Exporter::SetVertexTexture(texCoord.x,						texCoord.y);
				verttex += Exporter::SetVertexTexture(texCoord.x + tileSizeTexCoord.x,	texCoord.y);
				verttex += Exporter::SetVertexTexture(texCoord.x + tileSizeTexCoord.x,	texCoord.y + tileSizeTexCoord.y);
				verttex += Exporter::SetVertexTexture(texCoord.x,						texCoord.y + tileSizeTexCoord.y);

				face += Exporter::SetFace(nvertex, nvertex + 2, nvertex + 1, nvertex, nvertex + 2, nvertex + 3);
				face += Exporter::SetFace(nvertex, nvertex + 3, nvertex + 2, nvertex, nvertex + 1, nvertex + 2);

				nvertex += 4;
			}

			file += verttex;
			file += face;
		}

		++l_index;
	}
	ntexture = nvertex;

	file += "\n# Terrains and Objects\n";
	r1Tileset3d* tileset3d = App->gui->objects->tileset;
	std::map<uint64_t, unsigned int> textures;
	std::vector<std::string> texturesNames;
	std::queue<Object*> rootObjects;
	for (auto& l : layers) {
		if (l->type != MapLayer::Type::OBJECT)
			continue;
		for (int i = 0; i < size.x * size.y; ++i) {
			if (l->data[i] == 0U)
				continue;

			const r1Tileset3d::Tile3d* tile = tileset3d->tiles[l->data[i] - 1U];
			Resource* const obj = App->resources->Get(tile->uidObject);

			if (obj == nullptr)
				continue;

			if (obj->GetType() == Resource::Type::Object) {
				const r1Object* o = static_cast<r1Object*>(obj);

				std::string mtlTexture;
				auto iterTex = textures.find(o->texture->GetUID());
				if (iterTex == textures.end()) { //Texture not loaded
					std::string texname = o->texture->name + "." + o->texture->extension;
					int texIndex = 1;
					for (auto texn = texturesNames.begin(); texn != texturesNames.end(); ++texn) {
						if (texname.compare(*texn) == 0) {
							texname = o->texture->name + std::to_string(texIndex) + "." + o->texture->extension;
							texIndex++;
						}
					}
					mtlTexture = FileSystem::GetNameFile(texname.c_str());
					texturesNames.push_back(mtlTexture);
					textures.emplace(o->texture->GetUID(), texturesNames.size() - 1);
					mtl += "newmtl " + mtlTexture + "\n";
					mtl += "map_Kd " + texname + "\n\n";
					FileSystem::CopyTo(o->texture->path.c_str(), ("Export/" + texname).c_str());
				}
				else {
					mtlTexture = texturesNames[(*iterTex).second];
				}
				file += "o " + o->name + "\n";
				auto& mat = tile->transform.GetGlobalMatrix();
				float2 colrow = { float(i % size.x), float(i / size.x) };
				std::string vt;
				for (int v = 0; v < o->bvertices.size; ++v) {
					float3 local = { o->bvertices.data[v * 3 + 2], o->bvertices.data[v * 3 + 1], o->bvertices.data[v * 3] };
					auto pos = mat.MulPos(local);
					//file += Exporter::SetVertex(pos.x, pos.y, pos.z);
					file += Exporter::SetVertex(colrow.y + pos.x, l->height + pos.y, colrow.x + pos.z);
					vt += Exporter::SetVertexTexture(o->btexture.data[v * 2], o->btexture.data[v * 2 + 1]);
				}
				file += vt;
				file += "usemtl " + mtlTexture + "\n";
				for (int in = 0; in < o->bindices.size; in += 3) {
					file += Exporter::SetFace(nvertex + o->bindices.data[in], nvertex + o->bindices.data[in + 2], nvertex + o->bindices.data[in + 1],
						ntexture + o->bindices.data[in], ntexture + o->bindices.data[in + 2], ntexture + o->bindices.data[in + 1]);
				}
				nvertex += o->bvertices.size;
				ntexture += o->bvertices.size;
			}
			if (obj->GetType() == Resource::Type::Model) {
				auto o = static_cast<r1Model*>(obj);

				for (auto& m : o->meshes) {
					std::string mtlTexture;
					auto texture = (r1Texture*)App->resources->Get(static_cast<r1Model*>(obj)->materials[m->tex_i]);
					auto iterTex = textures.find(texture->GetUID());
					if (iterTex == textures.end()) { //Texture not loaded
						std::string texname = texture->name + "." + texture->extension;
						int texIndex = 1;
						for (auto texn = texturesNames.begin(); texn != texturesNames.end(); ++texn) {
							if (texname.compare(*texn) == 0) {
								texname = texture->name + std::to_string(texIndex) + "." + texture->extension;
								texIndex++;
							}
						}
						mtlTexture = FileSystem::GetNameFile(texname.c_str());
						texturesNames.push_back(mtlTexture);
						textures.emplace(texture->GetUID(), texturesNames.size() - 1);
						mtl += "newmtl " + mtlTexture + "\n";
						mtl += "map_Kd " + texname + "\n\n";
						FileSystem::CopyTo(texture->path.c_str(), ("Export/" + texname).c_str());
					}
					else {
						mtlTexture = texturesNames[(*iterTex).second];
					}
					file += "o " + o->name + "\n";
					float4x4 mat = tile->transform.GetGlobalMatrix();
					float2 colrow = { float(i % size.x), float(i / size.x) };
					std::string vt;
					for (int v = 0; v < m->vertices.size; ++v) {
						float3 local = { m->vertices.data[v * 3], m->vertices.data[v * 3 + 1], m->vertices.data[v * 3 + 2] };
						auto pos = float4x4::FromTRS(float3::zero, Quat::identity, float3(-1.f, 1.f, 1.f)).MulPos((Quat::RotateAxisAngle(float3::unitY, -math::pi / 2.f) * mat).MulPos(local));
						pos += float3(colrow.y, l->height, colrow.x);
						file += Exporter::SetVertex(pos.x, pos.y, pos.z);
						vt += Exporter::SetVertexTexture(m->texture.data[v * 2], m->texture.data[v * 2 + 1]);
					}
					file += vt;
					file += "usemtl " + mtlTexture + "\n";
					for (int in = 0; in < m->indices.size; in += 3) {
						file += Exporter::SetFace(nvertex + m->indices.data[in], nvertex + m->indices.data[in + 2], nvertex + m->indices.data[in + 1],
							ntexture + m->indices.data[in], ntexture + m->indices.data[in + 2], ntexture + m->indices.data[in + 1]);
					}
					nvertex += m->vertices.size;
					ntexture += m->vertices.size;
				}
			}
		}

		Object* root = static_cast<MapLayerTerrain*>(l)->root;
		if (!root->children.empty())
			rootObjects.push(root);
	}

	file += "\n# Models\n";

	while (!rootObjects.empty()) {
		std::queue<Object*> objects;
		Object* root = rootObjects.front();
		objects.push(root);
		rootObjects.pop();
		file += "o " + std::string(root->GetName()) + "\n";

		while (!objects.empty()) {
			Object* o = objects.front();
			objects.pop();
			for (auto& c : o->children)
				objects.push(c);

			auto m = (c1Mesh*)o->GetComponent(Component::Type::Mesh);
			if (m == nullptr)
				continue;
			
			std::string mtlTexture;
			auto texture = (r1Texture*)App->resources->Get(m->material->tex);
			if (auto iterTex = textures.find(texture->GetUID()); iterTex == textures.end()) { //Texture not loaded
				std::string texname = texture->name + "." + texture->extension;
				int texIndex = 1;
				for (auto texn = texturesNames.begin(); texn != texturesNames.end(); ++texn) {
					if (texname.compare(*texn) == 0) {
						texname = texture->name + std::to_string(texIndex) + "." + texture->extension;
						texIndex++;
					}
				}
				mtlTexture = FileSystem::GetNameFile(texname.c_str());
				texturesNames.push_back(mtlTexture);
				textures.emplace(texture->GetUID(), texturesNames.size() - 1);
				mtl += "newmtl " + mtlTexture + "\n";
				mtl += "map_Kd " + texname + "\n\n";
				FileSystem::CopyTo(texture->path.c_str(), ("Export/" + texname).c_str());
			}
			else {
				mtlTexture = texturesNames[(*iterTex).second];
			}

			std::string vt;
			auto rm = (r1Mesh*)App->resources->Get(m->GetMesh());
			float4x4 mat = ((c1Transform*)o->GetComponent(Component::Type::Transform))->GetGlobalMatrix();
			for (int v = 0; v < rm->vertices.size; ++v) {
				float3 local = { rm->vertices.data[v * 3 + 2], rm->vertices.data[v * 3 + 1], rm->vertices.data[v * 3] };
				float3 pos, scale;
				Quat rot;
				mat.Decompose(pos, rot, scale);
				local = (float4x4::FromTRS({ pos.z, pos.y, pos.x }, rot, scale)).MulPos(local);
				file += Exporter::SetVertex(local.x, local.y, local.z);
				vt += Exporter::SetVertexTexture(rm->texture.data[v * 2], rm->texture.data[v * 2 + 1]);
			}
			file += vt;

			file += "usemtl " + mtlTexture + "\n";
			for (int in = 0; in < rm->indices.size; in += 3) {
				file += Exporter::SetFace(nvertex + rm->indices.data[in], nvertex + rm->indices.data[in + 2], nvertex + rm->indices.data[in + 1],
					ntexture + rm->indices.data[in], ntexture + rm->indices.data[in + 2], ntexture + rm->indices.data[in + 1]);
			}
			nvertex += rm->vertices.size;
			ntexture += rm->vertices.size;
		}
	}

	FileSystem::SaveTextFile("Export/test.obj", file.c_str());
	FileSystem::SaveTextFile("Export/test.mtl", mtl.c_str());
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
		MapLayer* layer = App->map_editor->AddLayer(MapLayer::StringToType((*l).value("type", "tile")));
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
		queue.push({ col, row });
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