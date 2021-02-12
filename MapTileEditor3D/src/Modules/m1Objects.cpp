#include "Modules/m1Objects.h"
#include "Objects/Object.h"
#include "Core/Application.h"
#include "Modules/m1Render3D.h"

#include "Resources/r1Shader.h"

#include "Tools/OpenGL/Viewport.h"

#include "Tools/System/Profiler.h"

#include "ExternalTools/mmgr/mmgr.h"

m1Objects::m1Objects(bool start_enabled) : Module("Objects", start_enabled)
{
}

m1Objects::~m1Objects()
{
}

Object* m1Objects::CreateEmptyObject(Object* parent, const char* name)
{
	return new Object(parent, name);
}

void m1Objects::DeleteObject(Object* obj)
{
	if (obj->parent != nullptr)
		obj->parent->children.erase(std::find(obj->parent->children.begin(), obj->parent->children.end(), obj));

	delete obj;
}

bool m1Objects::Start()
{
	return true;
}

UpdateStatus m1Objects::Update()
{
	PROFILE_FUNCTION();

	//if (layer_root_selected != nullptr) {
	//	App->render->GetViewport("scene")->Begin();

	//	auto shader = App->render->GetShader("default");
	//	shader->Use();

	//	//layer_root_selected->Update();

	//	shader->SetMat4("model", float4x4::identity);

	//	App->render->GetViewport("scene")->End();
	//}

	return UpdateStatus::UPDATE_CONTINUE;
}

const Object* m1Objects::GetSelected() const
{
	return selected;
}

void m1Objects::Unselect()
{
	selected = nullptr;
	layer_root_selected = nullptr;
}
