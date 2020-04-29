#include "ModuleObjects.h"
#include "Object.h"
#include "ExternalTools/par_shapes/par_shapes.h"

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
	par_shapes_mesh* m = par_shapes_create_cube();
	Object* obj = new Object();

	obj->vertices.size = m->npoints;
	obj->vertices.data = new float[m->npoints * 3];
	memcpy(obj->vertices.data, m->points, m->npoints * 3 * sizeof(float));


	obj->indices.size = m->ntriangles * 3;
	obj->indices.data = new unsigned int[obj->indices.size];
	memcpy(obj->indices.data, (unsigned int*)m->triangles, obj->indices.size * sizeof(unsigned int));

	par_shapes_free_mesh(m);

	obj->GenerateBuffers();

	objects.push_back(obj);
}

bool m1Objects::Start()
{
	CreateCube();

	return true;
}

UpdateStatus m1Objects::Update()
{
	for (auto i = objects.begin(); i != objects.end(); ++i) {
		(*i)->Draw();
	}

	return UpdateStatus::UPDATE_CONTINUE;
}
