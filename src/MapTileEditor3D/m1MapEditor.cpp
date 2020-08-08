#include "m1MapEditor.h"

#include <GL/glew.h>

#include "Application.h"
#include "m1GUI.h"
#include "p1Scene.h"
#include "p1Tileset.h"
#include "Viewport.h"

#include "Object.h"

#include "Chunk.h"

#include "c1Mesh.h"
#include "c1Transform.h"
#include "c1Material.h"
#include "r1Mesh.h"

#include "Logger.h"

m1MapEditor::m1MapEditor() : Module("MapEditor", true)
{
}

m1MapEditor::~m1MapEditor()
{
}

bool m1MapEditor::Start()
{
	panel_scene = App->gui->scene;
	panel_tileset = App->gui->tileset;

	chunks = new Chunk();

	/*map = new int[size.x * size.y];
	memset(map, 0, sizeof(int) * size.x * size.y);

	tiles = new Object * [size.x * size.y];
	for (int i = 0; i < size.x * size.y; ++i) {
		tiles[i] = new Object();
	}

	float* t = new float[2 * 4];
	t[0] = 0.f; t[1] = 1.f - 1.f / 625.f;
	t[2] = 0.125f; t[3] = 1.f - 1.f / 625.f;
	t[4] = 0.125f; t[5] = 1.f;
	t[6] = 0.f; t[7] = 1.f;
	for (int i = 0; i < size.x * size.y; ++i) {
		tiles[i]->transform->SetPosition(i / size.x, 0.f, i % size.x);

		tiles[i]->CreateComponent<c1Mesh>()->SetEMesh(m1Resources::EResourceType::TILE);

		tiles[i]->GetComponent<c1Material>()->SetTexture("test2");
		auto mesh = tiles[i]->GetComponent<c1Mesh>();
		glBindBuffer(GL_ARRAY_BUFFER, mesh->emesh->texture.id);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * mesh->emesh->texture.size, t, GL_STATIC_DRAW);
	}
	delete[] t;*/

	return true;
}

UpdateStatus m1MapEditor::Update()
{
	panel_scene->viewport->Begin();

	panel_tileset->SelectTex();

	chunks->Update();

	glBindTexture(GL_TEXTURE_2D, NULL);

	panel_scene->viewport->End();

	return UpdateStatus::UPDATE_CONTINUE;
}

bool m1MapEditor::CleanUp()
{
	delete chunks;
	/*delete[] map;
	for (int i = 0; i < size.x * size.y; ++i) {
		delete tiles[i];;
	}
	delete[] tiles;*/

	return true;
}
