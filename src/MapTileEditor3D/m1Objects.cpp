#include "m1Objects.h"
#include "Object.h"
#include "c1Mesh.h"
#include "c1Material.h"
#include "Application.h"
#include "m1Render3D.h"
#include "ExternalTools/par_shapes/par_shapes.h"

#include "ExternalTools/mmgr/mmgr.h"

m1Objects::m1Objects(bool start_enabled) : Module("Objects", start_enabled)
{
}

m1Objects::~m1Objects()
{
	for (auto i = objects.begin(); i != objects.end(); ++i) {
		delete* i;
	}
}

void m1Objects::CreateCube()
{
	/*par_shapes_mesh* m = par_shapes_create_cube();
	//par_shapes_mesh* m = par_shapes_create_parametric_sphere(20,20);
	Object* obj = new Object();

	c1Mesh* mesh = (c1Mesh*)obj->CreateComponent(Component::Type::Mesh);
	
	mesh->vertices.size = m->npoints;
	mesh->vertices.data = new float[m->npoints * 3];
	memcpy(mesh->vertices.data, m->points, m->npoints * 3 * sizeof(float));

	mesh->indices.size = m->ntriangles * 3;
	mesh->indices.data = new unsigned int[mesh->indices.size];
	memcpy(mesh->indices.data, (unsigned int*)m->triangles, mesh->indices.size * sizeof(unsigned int));

	if (m->tcoords != nullptr) {
		mesh->texture.size = m->npoints;
		mesh->texture.data = new float[m->npoints * 2];
		memcpy(mesh->texture.data, m->tcoords, mesh->texture.size * 2 * sizeof(float));
	}
	
	par_shapes_free_mesh(m);

	mesh->GenerateBuffers();

	mesh->material->shader = App->render->bShader;

	objects.push_back(obj);*/
}

bool m1Objects::Start()
{
	CreateCube();

	return true;
}

UpdateStatus m1Objects::Update()
{
	for (auto i = objects.begin(); i != objects.end(); ++i) {
		(*i)->Update();
	}

	return UpdateStatus::UPDATE_CONTINUE;
}
