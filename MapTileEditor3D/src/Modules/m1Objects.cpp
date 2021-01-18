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

Object* m1Objects::CreateEmptyObject()
{
	return new Object();
}

bool m1Objects::Start()
{
	return true;
}

UpdateStatus m1Objects::Update()
{
	PROFILE_FUNCTION();

	if (layer_root_selected != nullptr) {
		App->render->GetViewport("scene")->Begin();

		auto shader = App->render->GetShader("default");
		shader->Use();

		layer_root_selected->Update();

		shader->SetMat4("model", float4x4::identity);

		App->render->GetViewport("scene")->End();
	}

	return UpdateStatus::UPDATE_CONTINUE;
}

const Object* m1Objects::GetSelected() const
{
	return selected;
}
