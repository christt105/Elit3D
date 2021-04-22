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
	if (tile_data)
		delete[] tile_data;
}

void MapLayerTerrain::Draw(const int2& size, int tile_width, int tile_height) const
{
	oglh::DepthEnable(true);
	static auto shader = App->render->GetShader("default");
	shader->Use();

	if (root != nullptr)
		root->Update();

	if (tile_data == nullptr)
		return;

	r1Tileset3d* tileset = App->gui->objects->tileset;
	for (int i = size.x * size.y - 1; i >= 0; --i) {
		if (tile_data[i] == 0U)
			continue;
		if (tile_data[i] > tileset->tiles.size())
			continue;
		const r1Tileset3d::Tile3d* tile = tileset->tiles[tile_data[i] - 1];
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

	if (tile_data != nullptr)
		delete[] tile_data;
	tile_data = new TILE_DATA_TYPE[size.x * size.y];
	memset(tile_data, 0, sizeof(TILE_DATA_TYPE) * size.x * size.y);
}

void MapLayerTerrain::Resize(const int2& oldSize, const int2& newSize)
{
	TILE_DATA_TYPE* new_data = new TILE_DATA_TYPE[newSize.x * newSize.y];
	memset(new_data, 0, sizeof(TILE_DATA_TYPE) * newSize.x * newSize.y);

	for (int i = 0; i < oldSize.x * oldSize.y; ++i) {
		int2 colrow = int2(i % oldSize.x, (i / oldSize.x));
		if (colrow.x < newSize.x && colrow.y < newSize.y) {
			int new_index = (colrow.x + newSize.x * colrow.y);
			int old_index = (colrow.x + oldSize.x * colrow.y);
			new_data[new_index] = tile_data[old_index];
		}
	}

	delete[] tile_data;
	tile_data = new_data;
}

std::string MapLayerTerrain::Parse(int sizeX, int sizeY, DataTypeExport d) const
{
	std::string ret;

	if (d != MapLayer::DataTypeExport::CSV_NO_NEWLINE)
		ret = '\n';

	for (int i = sizeY - 1; i >= 0; --i) {
		for (int j = 0; j < sizeX; ++j) {
			ret.append(std::to_string(tile_data[i * sizeX + j]) + ','); // TODO: encode 4 bytes array
		}

		if (i == 0)
			ret.pop_back();
		if (d != MapLayer::DataTypeExport::CSV_NO_NEWLINE)
			ret += '\n';
	}

	switch (d)
	{
	case MapLayer::DataTypeExport::BASE64_NO_COMPRESSION:
		ret = base64_encode(ret);
		break;
	case MapLayer::DataTypeExport::BASE64_ZLIB:
		ret = compress_string(ret);
		ret = base64_encode(ret);
		break;
	default:
		break;
	}

	return ret;
}

nlohmann::json MapLayerTerrain::Parse(int sizeX, int sizeY) const
{
	nlohmann::json ret;

	for (int i = sizeY - 1; i >= 0; --i) {
		for (int j = 0; j < sizeX; ++j) {
			ret.push_back(tile_data[i * sizeX + j]); // TODO: encode 4 bytes array
		}
	}

	return ret;
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

void MapLayerTerrain::Unparse(int sizeX, int sizeY, const std::string& raw_data)
{
	std::string data = decompress_string(base64_decode(raw_data));
	auto i = data.begin();
	if (*i == '\n')
		++i;
	int x = 0;
	int y = sizeY - 1;
	while (i != data.end()) {
		std::string n;
		while (i != data.end() && *i != ',') {
			if (*i == '\n' && (i + 1) != data.end()) { // Weird way to load cause the origin on textures is Bottom-Left and not Top-Left. TODO?
				x = 0;
				--y;
				break;
			}
			n += *i;
			i++;
		}
		if (!n.empty()) {
			tile_data[sizeX * y + x] = (TILE_DATA_TYPE)std::stoul(n);
			++x;
		}
		if (i != data.end())
			i++;
	}
}

nlohmann::json MapLayerTerrain::Serialize(const int2& size) const
{
	nlohmann::json lay = MapLayer::Serialize(size);

	lay["root"] = root->Parse();

	lay["encoding"] = "base64-zlib";
	lay["data"] = Parse(size.x, size.y, MapLayer::DataTypeExport::BASE64_ZLIB);

	return lay;
}

void MapLayerTerrain::Deserialize(const nlohmann::json& json, const int2& size)
{
	MapLayer::Deserialize(json, size);

	root->Unparse(json["root"]);
	Unparse(size.x, size.y, json.value("data", "0")/*TODO: unparse type (csv, base64, zlib...)*/);
}

