#include "c1Mesh.h"
#include <GL/glew.h>

#include "Application.h"
#include "m1Resources.h"

#include "Object.h"

#include "c1Material.h"
#include "c1Transform.h"

#include "r1Mesh.h"
#include "r1Shader.h"

#include "Logger.h"

#include "ExternalTools/mmgr/mmgr.h"

c1Mesh::c1Mesh(Object* obj) : Component(obj, Type::Mesh), material(obj->CreateComponent<c1Material>())
{
}

c1Mesh::~c1Mesh()
{
}

void c1Mesh::Update()
{
	Draw();
}

void c1Mesh::SetMesh(const uint64_t& id)
{
	mesh = id;
	Resource* res = App->resources->Get(mesh);
	res->Attach();
}

void c1Mesh::Draw()
{
	const r1Mesh* rmesh = (r1Mesh*)App->resources->Get(mesh);
	if (rmesh != nullptr) {
		glBindVertexArray(rmesh->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, rmesh->vertices.id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rmesh->indices.id);
		//glVertexPointer(3, GL_FLOAT, 0, (void*)0);
		//material->shader->SetVec3("color", float3::zero);
		material->shader->SetMat4("model", object->transform->mat);

		material->BindTex();
		glDrawElements(GL_TRIANGLES, rmesh->indices.size, GL_UNSIGNED_INT, (void*)0);
	}
	else
		LOGE("Object %s not find mesh %" SDL_PRIu64, object->GetName(), mesh)
}
