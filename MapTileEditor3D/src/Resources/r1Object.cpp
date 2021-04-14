#include "Resources/r1Object.h"
#include "Tools/OpenGL/ObjectEditor.h"
#include "Core/Application.h"
#include "Modules/m1Resources.h"
#include "Tools/FileSystem.h"
#include "Resources/r1Texture.h"

#include "ExternalTools/mmgr/mmgr.h"

r1Object::r1Object(const uint64_t& uid) : Resource(Resource::Type::Object, uid) {}

r1Object::~r1Object() {
	for (auto i = meshes.begin(); i != meshes.end(); ++i) {
		delete* i;
	}

	if (bvertices.data != nullptr) {
		oglh::DeleteVAO(VAO, bvertices.id, bindices.id);
		oglh::DeleteBuffer(btexture.id);

		delete[] bvertices.data;
		bvertices.data = nullptr;
		delete[] bindices.data;
		bindices.data = nullptr;
		delete[] btexture.data;
		btexture.data = nullptr;
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

		for (int t = 0; t < 2 * 4; ++t) {
			obj->uv.data[t] = json["texCoords"][t + i * 2 * 4];
		}

		oglh::BindTexture(obj->uv.id);
		oglh::SetArrayBuffer(obj->uv.id, obj->uv.size, sizeof(float), 2, obj->uv.data, 1, 2);

		meshes.push_back(obj);
	}

	if (texture = (r1Texture*)App->resources->Get(json["texture"]))
		texture->Attach();

	bvertices.size = json["vertices"].size()/3;
	bvertices.data = new float[bvertices.size * 3];
	i = 0;
	for (auto v = json["vertices"].begin(); v != json["vertices"].end(); ++v) {
		bvertices.data[i++] = *v;
	}

	bindices.size = bvertices.size / 2 * 3;
	bindices.data = new unsigned int[bindices.size];
	i = 0;
	for (auto in = 0; in < bindices.size; in += 6, ++i) {
		int offset = i * 4;
		bindices.data[in    ] = 0 + offset;
		bindices.data[in + 1] = 2 + offset;
		bindices.data[in + 2] = 1 + offset;
		bindices.data[in + 3] = 0 + offset;
		bindices.data[in + 4] = 3 + offset;
		bindices.data[in + 5] = 2 + offset;
	}

	btexture.size = json["texCoords"].size() / 2;
	btexture.data = new float[btexture.size * 2];
	i = 0;
	for (auto v = json["texCoords"].begin(); v != json["texCoords"].end(); ++v) {
		btexture.data[i++] = *v;
	}

	oglh::GenVAO(VAO);
	oglh::GenArrayBuffer(bvertices.id, bvertices.size, sizeof(float), 3, bvertices.data, 0, 3);
	oglh::GenElementBuffer(bindices.id, bindices.size, bindices.data);
	oglh::GenArrayBuffer(btexture.id, btexture.size, sizeof(float), 2, btexture.data, 1, 2);
}

void r1Object::Unload()
{
	for (auto m : meshes)
		delete m;
	meshes.clear();

	if (texture)
		texture->Detach();

	oglh::DeleteVAO(VAO, bvertices.id, bindices.id);
	oglh::DeleteBuffer(btexture.id);

	delete[] bvertices.data;
	bvertices.data = nullptr;
	delete[] bindices.data;
	bindices.data = nullptr;
	delete[] btexture.data;
	btexture.data = nullptr;
}
