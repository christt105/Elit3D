#include "Resources/r1Object.h"
#include "Tools/OpenGL/ObjectEditor.h"
#include "Core/Application.h"
#include "Modules/m1Resources.h"
#include "Tools/FileSystem.h"
#include "Resources/r1Texture.h"

r1Object::r1Object(const uint64_t& uid) : Resource(Resource::Type::Object, uid) {}

r1Object::~r1Object() {
	for (auto i = meshes.begin(); i != meshes.end(); ++i) {
		delete* i;
	}
}

void r1Object::Load()
{
	auto json = FileSystem::OpenJSONFile(path.c_str());

	int i = 0;
	for (auto m = json["meshes"].begin(); m != json["meshes"].end(); ++m, ++i) {
		ObjectEditor* obj = new ObjectEditor();
		obj->name = (*m).value("name", "mesh");
		obj->position = float3::FromString((*m).value("position", float3::zero.SerializeToString()));
		obj->rot = Quat::FromString((*m).value("rotation", Quat::identity.SerializeToString()));
		obj->size = float2::FromString((*m).value("size", float2::one.SerializeToString()));
		obj->scale = (*m).value("scale", 1.f);

		for (int v = 0; v < 3 * 4; ++v) {
			//obj->vertices.data[v] = json["vertices"][v + i * 3 * 4];
		}

		for (int t = 0; t < 2 * 4; ++t) {
			obj->uv.data[t] = json["texCoords"][t + i * 2 * 4];
		}

		oglh::BindTexture(obj->uv.id);
		oglh::SetArrayBuffer(obj->uv.id, obj->uv.size, sizeof(float), 2, obj->uv.data, 1, 2);

		meshes.push_back(obj);
	}

	if (texture = (r1Texture*)App->resources->Get(json["texture"]))
		texture->Attach();
}
