#include "Modules/m1MapEditor.h"

#include "Core/Application.h"
#include "Modules/m1GUI.h"
#include "Panels/p1Tileset.h"
#include "Panels/p1Layers.h"
#include "Panels/p1Tools.h"
#include "Panels/p1Inspector.h"
#include "Panels/p1Objects.h"
#include "Tools/OpenGL/Viewport.h"

#include "Modules/m1Render3D.h"
#include "Resources/r1Shader.h"

#include "Tools/Map/MapLayerTile.h"
#include "Tools/Map/MapLayerTerrain.h"

#include "Modules/m1Resources.h"
#include "Resources/r1Texture.h"
#include "Resources/r1Map.h"
#include "Resources/r1Mesh.h"
#include "Resources/r1Model.h"

#include "Modules/m1Camera3D.h"

#include "m1Objects.h"

#include "ExternalTools/MathGeoLib/include/Geometry/Plane.h"
#include "ExternalTools/MathGeoLib/include/Geometry/Ray.h"
#include "ExternalTools/MathGeoLib/include/Math/Quat.h"

#include "Modules/m1Input.h"

#include "Tools/System/Logger.h"

#include <GL/glew.h>

#include "Tools/OpenGL/OpenGLHelper.h"

#include "Tools/FileSystem.h"

#include "Tools/System/Profiler.h"

#include "ExternalTools/mmgr/mmgr.h"

m1MapEditor::m1MapEditor() : Module("MapEditor", true)
{
}

m1MapEditor::~m1MapEditor()
{
}

bool m1MapEditor::Start()
{
	PROFILE_FUNCTION();
	
	panel_tileset = App->gui->tileset;
	panel_layers = App->gui->layers;
	panel_tools = App->gui->tools;

	nlohmann::json locals = FileSystem::OpenJSONFile("prefs/locals.json");
	if (!locals.is_null()) {
		if (locals.find("last_map_used") != locals.end()) {
			if (locals.value("last_map_used", 0ULL) != 0ULL) {
				LoadMap(locals.value("last_map_used", 0ULL));
				App->camera->FitOnMap();
			}
		}
	}

	auto r = (r1Mesh*)App->resources->EGet(m1Resources::EResourceType::TILE);
	r->Attach();

	return true;
}

UpdateStatus m1MapEditor::Update()
{
	PROFILE_FUNCTION();

	if (map == 0ULL)
		return UpdateStatus::UPDATE_CONTINUE;

	auto m = (r1Map*)App->resources->Get(map);
	if (m == nullptr) {
		map = 0ULL;
		panel_tileset->SelectTileset(0ULL);
		return UpdateStatus::UPDATE_CONTINUE;
	}

	App->render->GetViewport("scene")->Begin();

	static auto shader = App->render->GetShader("tilemap");
	shader->Use();

	MapLayer::SelectBuffers();
	oglh::ActiveTexture(0);
	if (panel_tileset->SelectTex()) {
		shader->SetBool("tilemap_selected", true);
		panel_tileset->SelectTransparentColor(shader);
		shader->SetInt("tileAtlas", 0); // for now we only can draw a map with a single texture (TODO)
		shader->SetInt2("ntilesMap", m->size);
		shader->SetBool("debugPaint", App->debug.draw_debug_map);
		oglh::ActiveTexture(1);
		shader->SetInt("tilemap", 1);
		panel_tileset->SetColumnUniform(shader);

		int tileWidth = panel_tileset->GetTileWidth();
		int tileHeight = panel_tileset->GetTileHeight();
		for (auto layer = m->layers.rbegin(); layer != m->layers.rend(); ++layer) {
			if ((*layer)->type == MapLayer::Type::TILE && (*layer)->visible) {
				(*layer)->Draw(m->size, tileWidth, tileHeight);
			}
		}

		for (int i = 0; i < 2; ++i) {
			oglh::ActiveTexture(i);
			oglh::UnBindTexture();
		}

		oglh::ActiveTexture(0);

		for (auto layer = m->layers.begin(); layer != m->layers.end(); ++layer) { //TODO: Draw all tiles at the same time
			if (((*layer)->type == MapLayer::Type::OBJECT) && (*layer)->visible) {
				(*layer)->Draw(m->size, panel_tileset->GetTileWidth(), panel_tileset->GetTileWidth());
			}
		}
	}
	else {
		shader->SetBool("tilemap_selected", false);
		MapLayer::DrawTile(m->size);
	}

	oglh::ActiveTexture(0);

	App->render->GetViewport("scene")->End();

	return UpdateStatus::UPDATE_CONTINUE;
}

bool m1MapEditor::CleanUp()
{
	PROFILE_FUNCTION();

	return true;
}

void m1MapEditor::SaveMap() const
{
	((r1Map*)App->resources->Get(map))->Save(panel_tileset->GetTileset());
}

void m1MapEditor::SaveImageMap() const
{
	((r1Map*)App->resources->Get(map))->SaveInImage();
}

void m1MapEditor::LoadMap(const uint64_t& id)
{
	if (map != id) {
		auto m = (r1Map*)App->resources->Get(id);
		if (m != nullptr) {
			if (map != 0ULL) {
				auto m = (r1Map*)App->resources->Get(map);
				m->Detach();
			}
			map = id;
			m->Attach();

			nlohmann::json locals;
			locals["last_map_used"] = id;
			FileSystem::SaveJSONFile("prefs/locals.json", locals);

			App->gui->inspector->SetSelected(m, p1Inspector::SelectedType::EDITOR_MAP);
			App->objects->Unselect();
		}
		else {
			LOGW("map with id %" PRIu64 " could not be loaded, not in resources", id);
		}
	}
}

void m1MapEditor::ReLoadMap()
{
	auto m = (r1Map*)App->resources->Get(map);
	App->resources->ReimportResource(m->path.c_str());
}

void m1MapEditor::Mouse(const Ray& ray)
{
	auto m = (r1Map*)App->resources->Get(map);
	if (m == nullptr)
		return;

	int selected = panel_layers->GetSelected();
	if (selected < 0 || selected >= m->layers.size())
		return;

	float t = 0.f;
	if (!Plane::IntersectLinePlane(float3(0.f, 1.f, 0.f), m->layers[selected]->height, ray.pos, ray.dir, t) && t >= 0.f)
		return;

			float3 position = ray.GetPoint(t);
			auto col = (int)floor(position.z);
			auto row = (int)floor(position.x);

	switch (m->layers[selected]->GetType())
	{
	case MapLayer::Type::TILE:
		MouseTile(m, (MapLayerTile*)m->layers[selected], { col, row });
		break;
	case MapLayer::Type::OBJECT:
		MouseTileTerrain(m, { col, row }, (MapLayerTerrain*)m->layers[selected]);
		break;
	}
}

void m1MapEditor::MouseTileTerrain(r1Map* m, const int2& tile, MapLayerTerrain* layer)
{
	//TODO Draw transparent

	if (App->input->IsMouseButtonPressed(1) && m->CheckBoundaries({ tile.y, tile.x }, 1, p1Tools::Tools::BRUSH, p1Tools::Shape::RECTANGLE) && !layer->locked) {
		switch (panel_tools->GetSelectedTool())
		{
		case p1Tools::Tools::ERASER: {
			layer->data[m->size.x * tile.x + tile.y] = 0ULL;
			break;
		}
		default:
			if (int obj = App->gui->objects->selected + 1;  obj != 0) {
				layer->data[m->size.x * tile.x + tile.y] = obj;
			}
			break;
		}
	}
}

void m1MapEditor::MouseTile(r1Map* m, MapLayerTile* layer, const int2& tile)
{
	if (!App->gui->tileset->SelectTex())
		return;

	auto r = (r1Mesh*)App->resources->EGet(m1Resources::EResourceType::TILE);

	oglh::ActiveTexture(0);

	int brushSize = panel_tools->GetToolSize();
	p1Tools::Tools tool = panel_tools->GetSelectedTool();
	p1Tools::Shape shape = panel_tools->GetToolShape();

	static r1Shader* shader = App->render->GetShader("selectTile");
	shader->Use();
	shader->SetInt2("tileSelected", panel_tileset->GetTileSelected());
	//if (tool != p1Tools::Tools::BUCKET) { TODO: Fix
	shader->SetMat4("model",
		float4x4::FromTRS(
			float3(tile.y - brushSize + 1 + brushSize / 2, layer->height, tile.x - brushSize + 1 + brushSize / 2),
			Quat::identity,
			float3(brushSize, 1.f, brushSize)));
	/*}
	else {
	shader->SetMat4("model",
	float4x4::FromTRS(
	float3::zero,
	Quat::identity,
	float3(m->size.x, 1.f, m->size.y)));
	}*/
	shader->SetInt("tool", (int)tool);
	shader->SetBool("locked", layer->locked);
	shader->SetInt("brushSize", brushSize);
	shader->SetInt("brushShape", (int)shape);
	if (tool == p1Tools::Tools::BUCKET) {
		oglh::ActiveTexture(1);
		shader->SetInt("TextureMap", 1);
		layer->SelectTex();
		shader->SetVec2("sizeMap", { (float)tile.x, (float)tile.y });
		shader->SetVec2("mousePos", { (float)tile.y, (float)tile.x });
	}

	oglh::BindBuffers(r->VAO, r->vertices.id, r->indices.id);
	oglh::DrawElements(r->indices.size);
	if (App->input->IsMouseButtonPressed(1) && !layer->locked && m->CheckBoundaries({ tile.y, tile.x }, brushSize, tool, shape)) {
		switch (tool)
		{
		case p1Tools::Tools::BUCKET:
		case p1Tools::Tools::BRUSH: {
			if (!App->input->IsMouseButtonUp(1)) {
				TILE_DATA_TYPE tile_id = panel_tileset->GetTileIDSelected();
				if (tile_id != 0) {

					// tile.y = A * 256 + B
					unsigned char A = 0;
					unsigned char B = 0;

					A = tile_id / UCHAR_MAX;
					B = tile_id % UCHAR_MAX;

					m->Edit(layer, tile.x, tile.y, brushSize, tool, shape, tile_id, A, B);
				}
			}
			break;
		}
		case p1Tools::Tools::ERASER:
			m->Edit(layer, tile.x, tile.y, brushSize, tool, shape, 0, 0, 0);
			break;
		case p1Tools::Tools::EYEDROPPER:
			if (App->input->IsMouseButtonUp(1)) {
				for (auto i = m->layers.rbegin(); i != m->layers.rend(); ++i) {
					if ((*i)->type != MapLayer::Type::TILE || !(*i)->visible || ((MapLayerTile*)(*i))->data[m->size.x * tile.x + tile.y] == 0)
						continue;

					panel_tileset->SetTileIDSelected(((MapLayerTile*)(*i))->data[m->size.x * tile.x + tile.y]);
					break;
				}
			}
			break;
		case p1Tools::Tools::RECTANGLE:
			break;
		default:
			break;
		}
	}
	if (tool == p1Tools::Tools::BUCKET) {
		oglh::UnBindTexture();
		oglh::ActiveTexture(0);
	}
}

/*void m1MapEditor::MouseTileObject(MapLayerObject* layer, const int2& tile, const int2& mapSize)
{
		if (App->input->IsMouseButtonPressed(1) && tile.y >= 0 && tile.x >= 0 && tile.y < mapSize.x && tile.x < mapSize.y && !layer->locked) {
			switch (panel_tools->GetSelectedTool())
			{
				/*case p1Tools::Tools::BRUSH:
					break;
			case p1Tools::Tools::ERASER:
				if (layer->object_tile_data[mapSize.x * tile.x + tile.y] != 0ULL) {
					if (Resource* res = App->resources->Get(layer->object_tile_data[mapSize.x * tile.x + tile.y]))
						res->Detach();
				}
				layer->object_tile_data[mapSize.x * tile.x + tile.y] = 0ULL;
				break;
				/*case p1Tools::Tools::BUCKET:
					break;
				case p1Tools::Tools::EYEDROPPER:
					break;
				case p1Tools::Tools::RECTANGLE:
					break;
			default:
				uint64_t tile_id = App->gui->objects->GetObjectSelected();
				if (tile_id != 0ULL && tile_id != layer->object_tile_data[mapSize.x * tile.x + tile.y]) {
					Resource* res = App->resources->Get(tile_id);
					if (res != nullptr) {
						res->Attach();
						layer->object_tile_data[mapSize.x * tile.x + tile.y] = tile_id;
					}
				}
				break;
			}
		}
}*/

void m1MapEditor::ResizeMap(int width, int height)
{
	auto m = (r1Map*)App->resources->Get(map);
	if (m)
		m->Resize(width, height);
}

int2 m1MapEditor::GetMapSize() const
{
	if (auto m = (r1Map*)App->resources->Get(map))
		return m->size;
	return int2(-1, -1);
}

void m1MapEditor::ReorderLayers() const
{
	auto m = (r1Map*)App->resources->Get(map);
	if (m != nullptr)
		std::sort(m->layers.begin(), m->layers.end(), MapLayer::HeightOrder);
}

MapLayer* m1MapEditor::AddLayer(MapLayer::Type t)
{
	auto m = (r1Map*)App->resources->Get(map);
	if (m == nullptr)
		return nullptr;

	MapLayer* layer = nullptr;
	switch (t)
	{
	case MapLayer::Type::TILE:
		layer = new MapLayerTile(m);
		break;
	case MapLayer::Type::OBJECT:
		layer = new MapLayerTerrain(m);
		break;
	}
	
	if (layer == nullptr)
		return nullptr;

	layer->Reset(m->size);
	m->layers.push_back(layer);

	return layer;
}

void m1MapEditor::EraseLayer(int index)
{
	auto m = (r1Map*)App->resources->Get(map);
	if (m) {
		auto it = m->layers.begin() + index;
		delete* it;
		m->layers.erase(it);
	}
}

bool m1MapEditor::ValidMap() const
{
	return map != 0ULL;
}

r1Map* m1MapEditor::GetMap() const
{
	return (r1Map*)App->resources->Get(map);
}

bool m1MapEditor::GetLayers(std::vector<MapLayer*>* &vec) const
{
	auto m = (r1Map*)App->resources->Get(map);
	if (m == nullptr)
		return false;

	vec = &m->layers;

	return true;
}

MapLayerTerrain* m1MapEditor::GetObjectLayer(bool create_if_no_exist, bool select)
{
	auto m = GetMap();
	if(m == nullptr)
		return nullptr;

	if (int sel = panel_layers->GetSelected(); sel != -1)
		if (m->layers[sel]->type == MapLayer::Type::OBJECT)
			return (MapLayerTerrain*)m->layers[sel];

	for (int i = 0; i < m->layers.size(); ++i) {
		if (m->layers[i]->type == MapLayer::Type::OBJECT) {
			if (select)
				panel_layers->SetSelected(i);
			return (MapLayerTerrain*)m->layers[i];
		}
	}

	if (create_if_no_exist) {
		auto l = (MapLayerTerrain*)AddLayer(MapLayer::Type::OBJECT);
		if (select)
			panel_layers->SetSelected(m->layers.size()-1);
		return l;
	}

	return nullptr;
}

void m1MapEditor::ExportMap(MapTypeExport t, MapLayer::DataTypeExport d) const
{
	if (map != 0)
		((r1Map*)App->resources->Get(map))->Export(panel_tileset->GetTileset(), d, t);
}