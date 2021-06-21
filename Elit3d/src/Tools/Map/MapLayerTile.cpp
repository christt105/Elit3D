#include "MapLayerTile.h"

#include "Core/Application.h"
#include "Modules/m1Render3D.h"
#include "Resources/r1Shader.h"

#include "ExternalTools/MathGeoLib/include/Math/float4x4.h"
#include "ExternalTools/MathGeoLib/include/Math/Quat.h"

#include "ExternalTools/Assimp/include/scene.h"

#include "Tools/OpenGL/OpenGLHelper.h"
#include "ExternalTools/base64/base64.h"
#include "ExternalTools/zlib/zlib_strings.h"

#include "Resources/r1Map.h"

#include "Tools/System/Profiler.h"

#include "ExternalTools/mmgr/mmgr.h"

MapLayerTile::MapLayerTile(r1Map* map) : MapLayer(MapLayer::Type::TILE, map)
{
}

MapLayerTile::~MapLayerTile()
{
	if (data) {
		delete[] data;
		oglh::DeleteTexture(id_tex);
	}
}

void MapLayerTile::Draw(const int2& size, int tile_width, int tile_height) const
{
	oglh::BindTexture(id_tex);

	static auto shader = App->render->GetShader("tilemap");
	shader->SetMat4("model", float4x4::FromTRS(float3((float)displacement[0] / (float)tile_width, height, (float)displacement[1] / (float)tile_height), Quat::identity, float3((float)size.x, 1.f, (float)size.y))); // TODO: don't create a mat4x4 for every layer
	shader->SetFloat("alpha", opacity);
	oglh::DrawElements(tile.indices.size);
}

void MapLayerTile::Reset(const int2& size)
{
	if (data != nullptr)
		delete[] data;
	data = new TILE_DATA_TYPE[size.x * size.y];
	memset(data, 0, sizeof(TILE_DATA_TYPE) * size.x * size.y);

	unsigned char* tex = new unsigned char[size.x * size.y * 3];
	memset(tex, 0, sizeof(unsigned char) * size.x * size.y * 3);

	oglh::GenTextureData(id_tex, oglh::Wrap::Repeat, oglh::Filter::Nearest, size.x, size.y, tex);

	delete[] tex;
}

void MapLayerTile::Resize(const int2& oldSize, const int2& newSize)
{
	MapLayer::Resize(oldSize, newSize);

	{
		PROFILE_SECTION("Gen Texture");

		unsigned char* tex = new unsigned char[newSize.x * newSize.y * 3];
		memset(tex, 0, sizeof(unsigned char) * newSize.x * newSize.y * 3);

		for (auto i = 0; i < newSize.x * newSize.y; ++i) {
			if (data[i] != 0) {
				tex[i * 3] = (unsigned char)(data[i] / UCHAR_MAX);
				tex[i * 3 + 1] = 0;
				tex[i * 3 + 2] = (unsigned char)(data[i] % UCHAR_MAX);
			}
		}

		oglh::DeleteTexture(id_tex);
		oglh::GenTextureData(id_tex, oglh::Wrap::Repeat, oglh::Filter::Nearest, newSize.x, newSize.y, tex); //TODO: research a faster way to do this
		delete[] tex;
	}
}

void MapLayerTile::SelectTex() const
{
	oglh::BindTexture(id_tex);
}

aiNode* MapLayerTile::Parse(std::vector<aiMesh*>& meshes) const
{
	aiNode* ret = new aiNode();

	ret->mName = name;
	ret->mNumMeshes = 1;

	aiMesh* mesh = new aiMesh();
	mesh->mNumVertices = 4;
	mesh->mVertices = new aiVector3D[4]{
		{aiVector3D(0.f, 0.f, 0.f)},
		{aiVector3D(1.f, 0.f, 0.f)},
		{aiVector3D(1.f, 0.f, 1.f)},
		{aiVector3D(0.f, 0.f, 1.f)}
	};
	mesh->mNumFaces = 2;
	mesh->mFaces = new aiFace[2];
	mesh->mFaces[0].mNumIndices = 3;
	mesh->mFaces[0].mIndices = new unsigned int[3]{ 0, 1, 2 };
	mesh->mFaces[1].mNumIndices = 3;
	mesh->mFaces[1].mIndices = new unsigned int[3]{ 0, 2, 3 };
	mesh->mName = name;

	meshes.push_back(mesh);

	ret->mMeshes = new unsigned int[1];
	ret->mMeshes[0] = meshes.size() - 1;

	ret->mTransformation = aiMatrix4x4(aiVector3D(map->GetSize().x, 1, map->GetSize().y), aiQuaternion(), aiVector3D(displacement[0], height, displacement[1]));

	return ret;
}

void MapLayerTile::Unparse(const pugi::xml_node& node)
{
}

void MapLayerTile::Unparse(const nlohmann::json& node)
{
	MapLayer::Unparse(node);

	int2 size = map->GetSize();
	unsigned char* tex_data = new unsigned char[size.x * size.y * 3];
	memset(tex_data, 254, sizeof(unsigned char) * size.x * size.y * 3);

	for (int i = 0; i < size.x * size.y; ++i) {
		if (data[i] != 0) {
			tex_data[i * 3] = (unsigned char)(data[i] / UCHAR_MAX);
			tex_data[i * 3 + 1] = 0;
			tex_data[i * 3 + 2] = (unsigned char)(data[i] % UCHAR_MAX);
		}
	}

	oglh::GenTextureData(id_tex, oglh::Wrap::Repeat, oglh::Filter::Nearest, size.x, size.y, tex_data);
	oglh::UnBindTexture();
	delete[] tex_data;
}