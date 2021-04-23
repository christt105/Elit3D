#include "MapLayerTerrain.h"

#include "Core/Application.h"
#include "Modules/m1Render3D.h"
#include "Resources/r1Shader.h"

#include "Modules/m1GUI.h"

#include "Modules/m1Objects.h"
#include "Objects/Object.h"
#include "Panels/p1Objects.h"

#include "Modules/m1Resources.h"
#include "Resources/r1Tileset3d.h"
#include "Resources/r1Model.h"
#include "Resources/r1Mesh.h"
#include "Resources/r1Object.h"
#include "Resources/r1Texture.h"
#include "Resources/r1Map.h"

#include "ExternalTools/Assimp/include/scene.h"

#include "ExternalTools/base64/base64.h"
#include "ExternalTools/zlib/zlib_strings.h"

#include "Tools/OpenGL/OpenGLHelper.h"

#include "ExternalTools/mmgr/mmgr.h"

MapLayerTerrain::MapLayerTerrain(r1Map* map) : MapLayer(MapLayer::Type::OBJECT, map)
{
	root = App->objects->CreateEmptyObject(nullptr, "root");
}

MapLayerTerrain::~MapLayerTerrain()
{
	App->objects->DeleteObject(root);
	if (data)
		delete[] data;
}

void MapLayerTerrain::Draw(const int2& size, int tile_width, int tile_height) const
{
	oglh::DepthEnable(true);
	static auto shader = App->render->GetShader("default");
	shader->Use();

	if (root != nullptr)
		root->Update();

	if (data == nullptr)
		return;

	r1Tileset3d* tileset = App->gui->objects->tileset;
	for (int i = size.x * size.y - 1; i >= 0; --i) {
		if (data[i] == 0U)
			continue;
		if (data[i] > tileset->tiles.size()) //TODO: draw red cube
			continue;
		const r1Tileset3d::Tile3d* tile = tileset->tiles[data[i] - 1];
		Resource* obj = (Resource*)App->resources->Get(tile->uidObject);
		if (obj == nullptr)
			continue;
		if (obj->GetType() == Resource::Type::Model) {
			for (auto& m : static_cast<r1Model*>(obj)->meshes) {
				oglh::BindBuffers(m->VAO, m->vertices.id, m->indices.id);

				shader->SetMat4("model", float4x4::FromTRS(float3(i % size.x, height, i / size.x), Quat::identity, float3::one) * tile->transform.GetGlobalMatrix());

				auto tex = (r1Texture*)App->resources->Get(static_cast<r1Model*>(obj)->materials[m->tex_i]);
				if (tex != nullptr) tex->Bind();
				shader->SetBool("useTexture", m->texture.data != nullptr);
				oglh::DrawElements(m->indices.size);
				if (tex != nullptr) tex->Unbind();
			}
		}
		else if (obj->GetType() == Resource::Type::Object) {
			r1Object* o = static_cast<r1Object*>(obj);
			oglh::BindBuffers(o->VAO, o->bvertices.id, o->bindices.id);

			shader->SetMat4("model", float4x4::FromTRS({ (float)(i % size.x), height, (float)(i / size.x) }, Quat::identity, float3::one));

			if (o->texture != nullptr)
				o->texture->Bind();
			shader->SetBool("useTexture", o->btexture.data != nullptr);
			oglh::DrawElements(o->bindices.size);
			if (o->texture != nullptr)
				o->texture->Unbind();
		}

	}
	oglh::DepthEnable(false);
}

void MapLayerTerrain::Reset(const int2& size)
{
	if (root != nullptr)
		App->objects->DeleteObject(root);
	root = App->objects->CreateEmptyObject();

	if (data != nullptr)
		delete[] data;
	data = new TILE_DATA_TYPE[size.x * size.y];
	memset(data, 0, sizeof(TILE_DATA_TYPE) * size.x * size.y);
}

void MapLayerTerrain::Parse(pugi::xml_node& node, MapLayer::DataTypeExport t) const
{
	MapLayer::Parse(node, t);


}

void MapLayerTerrain::Parse(nlohmann::json& node, MapLayer::DataTypeExport t) const
{
	MapLayer::Parse(node, t);
	node["root"] = root->Parse();
}

aiNode* MapLayerTerrain::Parse(std::vector<aiMesh*>& meshes) const
{
	aiNode* ret = new aiNode();

	ret->mName = name;
	int2 size = map->GetSize();
	std::vector<unsigned int> objMeshes;

	/*for (int i = 0; i < size.x * size.y; ++i) {
		if (object_tile_data[i] == 0ULL) {
			continue;
		}

		r1Object* obj = (r1Object*)App->resources->Get(object_tile_data[i]);
		if (obj == nullptr)
			continue;

		aiMesh* mesh = new aiMesh();
		mesh->mName = obj->name;
		mesh->mNumVertices = obj->bvertices.size;
		mesh->mVertices = new aiVector3D[obj->bvertices.size];
		for (int j = 0; j < mesh->mNumVertices; ++j) {
			mesh->mVertices[j] = aiVector3D(i % size.x, height, i / size.x) + aiVector3D(obj->bvertices.data[j * 3], obj->bvertices.data[j * 3 + 1], obj->bvertices.data[j * 3 + 2]);
		}

		mesh->mNumFaces = obj->bindices.size / 3;
		mesh->mFaces = new aiFace[mesh->mNumFaces];
		for (int j = 0; j < mesh->mNumFaces; ++j) {
			mesh->mFaces[j].mNumIndices = 3;
			mesh->mFaces[j].mIndices = new unsigned int[3]{
				obj->bindices.data[j],
				obj->bindices.data[j * 3 + 2],
				obj->bindices.data[j * 3 + 1]
			};
		}

		meshes.push_back(mesh);
		objMeshes.push_back(meshes.size() - 1);
	}

	ret->mNumMeshes = objMeshes.size();
	ret->mMeshes = new unsigned int[ret->mNumMeshes];
	for (int i = 0; i < objMeshes.size(); ++i) {
		ret->mMeshes[i] = objMeshes[i];
	}*/

	return ret;
}

void MapLayerTerrain::Unparse(const pugi::xml_node& node)
{
}

void MapLayerTerrain::Unparse(const nlohmann::json& node)
{
	MapLayer::Unparse(node);

	root->Unparse(node["root"]);
}
