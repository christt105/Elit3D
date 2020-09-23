#include "m1MapEditor.h"

#include "Application.h"
#include "m1GUI.h"
#include "p1Tileset.h"
#include "Viewport.h"

#include "m1Render3D.h"
#include "r1Shader.h"

#include "MapLayer.h"

#include "m1Resources.h"
#include "r1Texture.h"
#include "r1Map.h"

#include "m1Input.h"

#include "Logger.h"

#include "OpenGLHelper.h"

#include "FileSystem.h"

#include "Profiler.h"

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

	return true;
}

UpdateStatus m1MapEditor::Update()
{
	PROFILE_FUNCTION();

	if (map == 0ULL) {
		LoadMap(App->resources->FindByName("map"));
	}
	else {
		App->render->GetViewport("scene")->Begin();

		static auto shader = App->render->GetShader("tilemap");
		shader->Use();

		auto m = (r1Map*)App->resources->Get(map);

		Layer::SelectBuffers();
		oglh::ActiveTexture(0);
		if (panel_tileset->SelectTex()) {
			shader->SetBool("tilemap_selected", true);
			panel_tileset->SelectTransparentColor(shader);
			shader->SetInt("tileAtlas", 0); // for now we only can draw a map with a single texture (TODO)
			shader->SetInt2("ntilesMap", m->size);
			oglh::ActiveTexture(1);
			shader->SetInt("tilemap", 1);

			auto layers = m->layers;
			std::sort(layers.begin(), layers.end(), Layer::HeightOrder); //TODO not every frame

			for (auto layer : layers) {
				if (layer->visible) {
					layer->Prepare();
					layer->Update(m->size);
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
	map = id;
	auto m = (r1Map*)App->resources->Get(id);
	m->Attach();
}

void m1MapEditor::ReLoadMap()
{
	auto m = (r1Map*)App->resources->Get(map);
	App->resources->ReimportResource(m->assets_path.c_str());
}

void m1MapEditor::MousePicking(const float3& position)
{
	int2 tile = panel_tileset->GetTileSelected();
	if (tile.x != -1 && tile.y != -1) {

		// tile.y = A * 256 + B
		char A = 0;
		char B = 0;

		A = tile.y / 256;
		B = tile.y % 256;

		auto m = (r1Map*)App->resources->Get(map);
		if (m) {
			int col = (int)floor(position.z);
			int row = (int)floor(-position.x);
			if (row < m->size.x && col < m->size.y && (col > -1 && row > -1)) {
				if (m->layers[0]->tile_data[(m->size.x * col + row) * 3	   ] != tile.x ||
					m->layers[0]->tile_data[(m->size.x * col + row) * 3 + 1] != A ||
					m->layers[0]->tile_data[(m->size.x * col + row) * 3 + 2] != B)
				{
					m->Edit(0, col, row, tile.x, A, B);
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

void m1MapEditor::AddLayer()
{
	auto m = (r1Map*)App->resources->Get(map);
	if (m) {
		Layer* layer = new Layer();
		layer->Reset(m->size);
		m->layers.push_back(layer);
	}
}

void m1MapEditor::EraseLayer(int index)
{
	auto m = (r1Map*)App->resources->Get(map);
	if (m) {
		auto it = m->layers.begin() + index;
		//delete* it;
		m->layers.erase(it);
	}
}

bool m1MapEditor::ValidMap() const
{
	return map != 0ULL;
}

bool m1MapEditor::GetLayers(std::vector<Layer*>& vec) const
{
	auto m = (r1Map*)App->resources->Get(map);
	if (m == nullptr)
		return false;

	vec = m->layers;

	return true;
}
