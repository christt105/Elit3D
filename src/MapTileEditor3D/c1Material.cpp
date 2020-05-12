#include "c1Material.h"

#include <GL/glew.h>

#include "Application.h"
#include "m1Resources.h"
#include "r1Texture.h"

#include "ExternalTools/mmgr/mmgr.h"

c1Material::c1Material(Object* obj) : Component(obj, Type::Material)
{
	tex = App->resources->Find("four_tiles");
	auto r = App->resources->Get(tex);
	r->Attach();
}

c1Material::~c1Material()
{
}

void c1Material::BindTex()
{
	auto t = (r1Texture*)App->resources->Get(tex);
	glBindTexture(GL_TEXTURE_2D, t->GetBufferID());
}
