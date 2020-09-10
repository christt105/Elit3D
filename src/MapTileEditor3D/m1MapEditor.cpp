#include "m1MapEditor.h"

#include <GL/glew.h>

#include "Application.h"
#include "m1GUI.h"
#include "p1Scene.h"
#include "p1Tileset.h"
#include "Viewport.h"

#include "m1Render3D.h"
#include "r1Shader.h"

#include "Object.h"

#include "MapLayer.h"

#include "c1Mesh.h"
#include "c1Transform.h"
#include "c1Material.h"
#include "r1Mesh.h"
#include "r1Texture.h"
#include "r1Map.h"

#include "m1Input.h"

#include "Logger.h"


#include "FileSystem.h"

#include "Profiler.h"

m1MapEditor::m1MapEditor() : Module("MapEditor", true)
{
}

m1MapEditor::~m1MapEditor()
{
}

bool m1MapEditor::Start()
{
	PROFILE_FUNCTION();

	panel_scene = App->gui->scene;
	panel_tileset = App->gui->tileset;

	return true;
}

UpdateStatus m1MapEditor::Update()
{
	PROFILE_FUNCTION();

	if (map == 0ULL) {
		LoadMap(App->resources->Find("map"));
	}
	else {

		panel_scene->viewport->Begin();

		static auto shader = App->render->GetShader("tilemap");
		shader->Use();

		Layer::SelectBuffers();
		glActiveTexture(GL_TEXTURE0 + 0);
		panel_tileset->SelectTex();
		panel_tileset->SelectTransparentColor(shader);
		shader->SetInt("tileAtlas", 0); // for now we only can draw a map with a single texture (TODO)
		shader->SetInt2("ntilesMap", { 10, 10 });
		glActiveTexture(GL_TEXTURE0 + 1);

		auto m = (r1Map*)App->resources->Get(map);
		for (auto layer : m->layers) {
			layer->Prepare();
			shader->SetInt("tilemap", 1);
			layer->Update();
		}

		for (int i = 0; i < 2; ++i) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, NULL);
		}

		glActiveTexture(GL_TEXTURE0);

		panel_scene->viewport->End();
	}

	return UpdateStatus::UPDATE_CONTINUE;
}

bool m1MapEditor::CleanUp()
{
	PROFILE_FUNCTION();

	/*for (auto i = layers.begin(); i != layers.end(); ++i)
		delete* i;*/

	return true;
}

void m1MapEditor::SaveMap()
{
	((r1Map*)App->resources->Get(map))->Save();
}

void m1MapEditor::LoadMap(const uint64_t& id)
{
	map = id;
	auto m = (r1Map*)App->resources->Get(id);
	m->Attach();
}

void m1MapEditor::MousePicking(const float3& position)
{
	int2 tile = panel_tileset->GetTileSelected();
	if (tile.x != -1 && tile.y != -1) {

		// tile.y = A * 256 + B
		int A = 0;
		int B = 0;

		A = tile.y / 256;
		B = tile.y % 256;

		auto m = (r1Map*)App->resources->Get(map);
		if (m) {
			int col = floor(position.z);
			int row = floor(-position.x);

			if (m->layers[0]->data[(m->size.x * col + row) * 3] != tile.x ||
				m->layers[0]->data[(m->size.x * col + row) * 3 + 1] != A ||
				m->layers[0]->data[(m->size.x * col + row) * 3 + 2] != B) 
			{
				m->Edit(0, floor(position.z), floor(-position.x), tile.x, A, B);
			}
		}
	}
}

bool m1MapEditor::ValidMap() const
{
	return map != 0ULL;
}
