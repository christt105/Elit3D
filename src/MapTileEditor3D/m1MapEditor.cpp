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

#include "m1Input.h"

#include "Logger.h"

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

	layers.push_back(new Layer());

	auto tex = (r1Texture*)App->resources->FindGet("testtilemap");
	if (tex != nullptr) {
		tex->Attach();
	}

	return true;
}

UpdateStatus m1MapEditor::Update()
{
	PROFILE_FUNCTION();
	
	panel_scene->viewport->Begin();

	static auto shader = App->render->GetShader("tilemap");
	shader->Use();

	Layer::SelectBuffers();
	glActiveTexture(GL_TEXTURE0 + 0);
	panel_tileset->SelectTex();
	shader->SetInt("tileAtlas", 0);

	glActiveTexture(GL_TEXTURE0 + 1);
	static auto tex = (r1Texture*)App->resources->FindGet("testtilemap");
	if (tex != nullptr) {
		glBindTexture(GL_TEXTURE_2D, tex->GetBufferID());
	}
	shader->SetInt("tilemap", 1);

	shader->SetInt2("ntilesMap", { 8, 6 });
	

	for(auto layer : layers)
		layer->Update();

	for (int i = 0; i < 2; ++i) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, NULL);
	}

	panel_scene->viewport->End();

	return UpdateStatus::UPDATE_CONTINUE;
}

bool m1MapEditor::CleanUp()
{
	PROFILE_FUNCTION();

	for (auto i = layers.begin(); i != layers.end(); ++i)
		delete* i;

	return true;
}

void m1MapEditor::MousePicking(const float3& position)
{
	auto tex = (r1Texture*)App->resources->FindGet("testtilemap");
	int2 tile = panel_tileset->GetTileSelected();
	if (tile.x != -1 || tile.y != -1)
		tex->Edit(floor(position.z), floor(-position.x), tile.x, tile.y, 1);
}
