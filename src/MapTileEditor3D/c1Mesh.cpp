#include "c1Mesh.h"
#include <GL/glew.h>

#include "Application.h"
#include "m1Resources.h"

#include "Object.h"
#include "c1Material.h"
#include "r1Mesh.h"
#include "r1Shader.h"

#include "Logger.h"

#include "ExternalTools/mmgr/mmgr.h"

c1Mesh::c1Mesh(Object* obj) : Component(obj, Type::Mesh)
{
	material = (c1Material*)obj->CreateComponent(Type::Material);
}

c1Mesh::~c1Mesh()
{
}

void c1Mesh::Update()
{
	Draw();
}

void c1Mesh::Draw()
{
	const r1Mesh* rmesh = (r1Mesh*)App->resources->Get(mesh);
	if (rmesh != nullptr) {
		glBindVertexArray(rmesh->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, rmesh->vertices.id);
		glVertexPointer(3, GL_FLOAT, 0, (void*)0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rmesh->indices.id);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		material->shader->SetVec3("color", float3::one);
		glDrawElements(GL_TRIANGLES, rmesh->indices.size, GL_UNSIGNED_INT, (void*)0);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glPolygonOffset(-1.f, 1.f);
		glLineWidth(1.5f);
		material->shader->SetVec3("color", float3::zero);
		glDrawElements(GL_TRIANGLES, rmesh->indices.size, GL_UNSIGNED_INT, (void*)0);
		glLineWidth(1.f);
	}
	else
		LOGE("Object %s not find mesh %" SDL_PRIu64, object->GetName(), mesh)
}
