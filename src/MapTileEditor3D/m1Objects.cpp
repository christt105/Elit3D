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

Object* m1Objects::CreateEmptyObject()
{
	Object* obj = new Object();
	objects.push_back(obj);

	return obj;
}

bool m1Objects::Start()
{
	return true;
}

UpdateStatus m1Objects::Update()
{
	for (auto i = objects.begin(); i != objects.end(); ++i) {
		(*i)->Update();
	}

	return UpdateStatus::UPDATE_CONTINUE;
}
