#include "MapLayerObject.h"

#include "Core/Application.h"
#include "Modules/m1Objects.h"
#include "Modules/m1Resources.h"
#include "Objects/Object.h"
#include "Resources/r1Object.h"
#include "Resources/r1Texture.h"

#include "ExternalTools/MathGeoLib/include/Math/Quat.h"

#include "Tools/OpenGL/OpenGLHelper.h"
#include "Modules/m1Render3D.h"
#include "Resources/r1Shader.h"
#include "Resources/r1Map.h"

#include "ExternalTools/Assimp/include/scene.h"

#include "ExternalTools/base64/base64.h"
#include "ExternalTools/zlib/zlib_strings.h"

#include "ExternalTools/mmgr/mmgr.h"

MapLayerObject::MapLayerObject(r1Map* map) : MapLayer(MapLayer::Type::OBJECT, map)
{
	root = App->objects->CreateEmptyObject(nullptr, "root");
}

MapLayerObject::~MapLayerObject()
{
	App->objects->DeleteObject(root);
	if (object_tile_data) {
		delete[] object_tile_data;
	}
}

void MapLayerObject::Draw(const int2& size, int tile_width, int tile_height) const
{
	if (root == nullptr)
		return;
	oglh::DepthEnable(true);
	static auto shader = App->render->GetShader("default");
	shader->Use();

	root->Update();

	for (int i = size.x * size.y - 1; i >= 0; --i) {
		if (object_tile_data[i] == 0ULL)
			continue;
		r1Object* obj = (r1Object*)App->resources->Get(object_tile_data[i]);
		if (obj == nullptr)
			continue;

		oglh::BindBuffers(obj->VAO, obj->bvertices.id, obj->bindices.id);

		shader->SetMat4("model", float4x4::FromTRS(float3(i % size.x, height, i / size.x), Quat::identity, float3::one));

		if (obj->texture != nullptr) obj->texture->Bind();
		shader->SetBool("useTexture", obj->texture != nullptr);
		oglh::DrawElements(obj->bindices.size);
		if (obj->texture != nullptr) obj->texture->Unbind();
	}
	oglh::DepthEnable(false);
	shader->SetMat4("model", float4x4::identity);
}

void MapLayerObject::Reset(const int2& size)
{
	if (root != nullptr)
		App->objects->DeleteObject(root);
	root = App->objects->CreateEmptyObject();
	if (object_tile_data != nullptr)
		delete[] object_tile_data;
	object_tile_data = new uint64_t[size.x * size.y];
	memset(object_tile_data, 0, sizeof(uint64_t) * size.x * size.y);
}

void MapLayerObject::Resize(const int2& oldSize, const int2& newSize)
{
	TILEOBJECT_DATA_TYPE* new_data = new TILEOBJECT_DATA_TYPE[newSize.x * newSize.y];
	memset(new_data, 0, sizeof(TILEOBJECT_DATA_TYPE) * newSize.x * newSize.y);

	for (int i = 0; i < oldSize.x * oldSize.y; ++i) {
		int2 colrow = int2(i % oldSize.x, (i / oldSize.x));
		if (colrow.x < newSize.x && colrow.y < newSize.y) {
			int new_index = (colrow.x + newSize.x * colrow.y);
			int old_index = (colrow.x + oldSize.x * colrow.y);
			new_data[new_index] = object_tile_data[old_index];
		}
	}

	delete[] object_tile_data;
	object_tile_data = new_data;
}

std::string MapLayerObject::Parse(int sizeX, int sizeY, DataTypeExport d) const
{
	std::string ret;

	if (d != MapLayer::DataTypeExport::CSV_NO_NEWLINE)
		ret = '\n';

	for (int i = sizeY - 1; i >= 0; --i) {
		for (int j = 0; j < sizeX; ++j) {
			ret.append(std::to_string(object_tile_data[i * sizeX + j]) + ','); // TODO: encode 4 bytes array
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

nlohmann::json MapLayerObject::Parse(int sizeX, int sizeY) const
{
	return nlohmann::json();
}

aiNode* MapLayerObject::Parse(std::vector<aiMesh*>& meshes) const
{
	aiNode* ret = new aiNode();

	ret->mName = name;
	int2 size = map->GetSize();
	std::vector<unsigned int> objMeshes;

	for (int i = 0; i < size.x * size.y; ++i) {
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
	}

	return ret;
}

void MapLayerObject::Unparse(int sizeX, int sizeY, const std::string& raw_data)
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
			uint64_t uid = std::stoull(n);
			if (uid != 0)
				if (Resource* res = App->resources->Get(uid)) res->Attach();
			object_tile_data[sizeX * y + x] = uid;
			++x;
		}
		if (i != data.end())
			i++;
	}
}

nlohmann::json MapLayerObject::Serialize(const int2& size) const
{
	nlohmann::json lay = MapLayer::Serialize(size);

	lay["root"] = root->Parse();
	lay["data"] = Parse(size.x, size.y, MapLayer::DataTypeExport::BASE64_ZLIB);

	return lay;
}

void MapLayerObject::Deserialize(const nlohmann::json& json, const int2& size)
{
	MapLayer::Deserialize(json, size);

	root->Unparse(json["root"]);
	Unparse(size.x, size.y, json.value("data", "0"));
}
