#include "Modules/m1MapEditor.h"

#include "Core/Application.h"
#include "Modules/m1GUI.h"
#include "Panels/p1Tileset.h"
#include "Panels/p1Layers.h"
#include "Panels/p1Tools.h"
#include "Panels/p1Inspector.h"
#include "Tools/OpenGL/Viewport.h"

#include "Modules/m1Render3D.h"
#include "Resources/r1Shader.h"

#include "Tools/Map/MapLayer.h"

#include "Modules/m1Resources.h"
#include "Resources/r1Texture.h"
#include "Resources/r1Map.h"
#include "Resources/r1Mesh.h"

#include "m1Objects.h"

#include "ExternalTools/MathGeoLib/include/Geometry/Plane.h"
#include "ExternalTools/MathGeoLib/include/Geometry/Ray.h"
#include "ExternalTools/MathGeoLib/include/Math/Quat.h"

#include "Modules/m1Input.h"

#include "Tools/System/Logger.h"

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
			if (locals.value("last_map_used", 0ULL) != 0ULL)
				LoadMap(locals.value("last_map_used", 0ULL));
		}
	}

	auto r = (r1Mesh*)App->resources->EGet(m1Resources::EResourceType::TILE);
	r->Attach();

	return true;
}

UpdateStatus m1MapEditor::Update()
{
	PROFILE_FUNCTION();

	if (map != 0ULL) {
		App->render->GetViewport("scene")->Begin();

		static auto shader = App->render->GetShader("tilemap");
		shader->Use();

		auto m = (r1Map*)App->resources->Get(map);
		if (m == nullptr) {
			map = 0ULL;
			panel_tileset->SelectTileset(0ULL);
			return UpdateStatus::UPDATE_CONTINUE;
		}

		Layer::SelectBuffers();
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

			for (auto layer = m->layers.begin(); layer != m->layers.end(); ++layer) {
				if ((*layer)->visible) {
					(*layer)->Draw(m->size, panel_tileset->GetTileWidth(), panel_tileset->GetTileWidth()); //TODO: optimize get tile width and height
				}
			}

			for (int i = 0; i < 2; ++i) {
				oglh::ActiveTexture(i);
				oglh::UnBindTexture();
			}
		}
		else {
			shader->SetBool("tilemap_selected", false);
			Layer::DrawTile(m->size);
		}

		oglh::ActiveTexture(0);

		App->render->GetViewport("scene")->End();
	}

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
	if (m) {
		if (App->gui->tileset->SelectTex()) {
			int index = panel_layers->GetSelected();
			float t = 0.f;
			auto r = (r1Mesh*)App->resources->EGet(m1Resources::EResourceType::TILE);
			if (index < (int)m->layers.size() && index > -1) {
				if (Plane::IntersectLinePlane(float3(0.f, 1.f, 0.f), m->layers[index]->height, ray.pos, ray.dir, t) && t > 0.f) {
					float3 position = ray.GetPoint(t);
					auto col = (int)floor(position.z);
					auto row = (int)floor(position.x);

					oglh::ActiveTexture(0);
					//select tileset

					int brushSize = panel_tools->GetToolSize();
					p1Tools::Tools tool = panel_tools->GetSelectedTool();
					p1Tools::Shape shape = panel_tools->GetToolShape();

					static r1Shader* shader = App->render->GetShader("selectTile");
					shader->Use();
					shader->SetInt2("tileSelected", panel_tileset->GetTileSelected());
					//if (tool != p1Tools::Tools::BUCKET) { TODO: Fix
						shader->SetMat4("model", 
							float4x4::FromTRS(
								float3(row - brushSize + 1 + brushSize / 2, m->layers[index]->height, col - brushSize + 1 + brushSize / 2), 
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
					shader->SetBool("locked", m->layers[index]->locked);
					shader->SetInt("brushSize", brushSize);
					shader->SetInt("brushShape", (int)shape);
					if (tool == p1Tools::Tools::BUCKET) {
						oglh::ActiveTexture(1);
						shader->SetInt("TextureMap", 1);
						m->layers[index]->SelectTex();
						shader->SetVec2("sizeMap", { (float)m->size.x, (float)m->size.y });
						shader->SetVec2("mousePos", { (float)row, (float)col });
					}

					oglh::BindBuffers(r->VAO, r->vertices.id, r->indices.id);
					oglh::DrawElements(r->indices.size);
					if (App->input->IsMouseButtonPressed(1) && !m->layers[index]->locked) {
						if (m->CheckBoundaries({ row, col }, brushSize, tool, shape)) {
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

										m->Edit(index, col, row, brushSize, tool, shape, tile_id, A, B);
									}
								}
								break;
							}
							case p1Tools::Tools::ERASER:
								m->Edit(index, col, row, brushSize, tool, shape, 0, 0, 0);
								break;
							case p1Tools::Tools::EYEDROPPER:
								if (App->input->IsMouseButtonUp(1)) {
									for (auto i = m->layers.rbegin(); i != m->layers.rend(); ++i) {
										if ((*i)->visible && (*i)->tile_data[m->size.x * col + row] != 0) {
											panel_tileset->SetTileIDSelected((*i)->tile_data[m->size.x * col + row]);
											break;
										}
									}
								}
								break;
							case p1Tools::Tools::RECTANGLE:
								break;
							default:
								break;
							}
						}
					}
					if (tool == p1Tools::Tools::BUCKET) {
						oglh::UnBindTexture();
						oglh::ActiveTexture(0);
					}
				}
			}
		}
	}
}

void m1MapEditor::ResizeMap(int width, int height)
{
	auto m = (r1Map*)App->resources->Get(map);
	if (m)
		m->Resize(width, height);
}

int2 m1MapEditor::GetMapSize() const
{
	auto m = (r1Map*)App->resources->Get(map);
	if (m)
		return m->size;
	return int2(-1, -1);
}

void m1MapEditor::ReorderLayers()
{
	auto m = (r1Map*)App->resources->Get(map);
	if (m != nullptr)
		std::sort(m->layers.begin(), m->layers.end(), Layer::HeightOrder);
}

Layer* m1MapEditor::AddLayer(Layer::Type t)
{
	auto m = (r1Map*)App->resources->Get(map);
	if (m) {
		Layer* layer = new Layer(t);
		if (t == Layer::Type::TILE) {
			layer->Reset(m->size);
		}
		else {
			layer->root = App->objects->CreateEmptyObject();
		}
		m->layers.push_back(layer);
		return layer;
	}
	return nullptr;
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

bool m1MapEditor::GetLayers(std::vector<Layer*>* &vec) const
{
	auto m = (r1Map*)App->resources->Get(map);
	if (m == nullptr)
		return false;

	vec = &m->layers;

	return true;
}

Layer* m1MapEditor::GetObjectLayer(bool create_if_no_exist, bool select)
{
	auto m = GetMap();
	if(m == nullptr)
		return nullptr;

	int sel = panel_layers->GetSelected();
	if (sel != -1)
		if (m->layers[sel]->type == Layer::Type::OBJECT)
			return m->layers[sel];

	for (int i = 0; i < m->layers.size(); ++i) {
		if (m->layers[i]->type == Layer::Type::OBJECT) {
			if (select)
				panel_layers->SetSelected(i);
			return m->layers[i];
		}
	}

	if (create_if_no_exist) {
		auto l = AddLayer(Layer::Type::OBJECT);
		if (select)
			panel_layers->SetSelected(m->layers.size()-1);
		return l;
	}

	return nullptr;
}

void m1MapEditor::ExportMap(MapTypeExport t, Layer::DataTypeExport d) const
{
	if (map != 0)
		((r1Map*)App->resources->Get(map))->Export(panel_tileset->GetTileset(), d, t);
}
