#include "MapLayerTile.h"

#include "Core/Application.h"
#include "Modules/m1Render3D.h"
#include "Resources/r1Shader.h"

#include "ExternalTools/MathGeoLib/include/Math/float4x4.h"
#include "ExternalTools/MathGeoLib/include/Math/Quat.h"

#include "Tools/OpenGL/OpenGLHelper.h"
#include "ExternalTools/base64/base64.h"
#include "ExternalTools/zlib/zlib_strings.h"

#include "Tools/System/Profiler.h"

#include "ExternalTools/mmgr/mmgr.h"

MapLayerTile::MapLayerTile() : MapLayer(MapLayer::Type::TILE)
{
}

MapLayerTile::~MapLayerTile()
{
	if (tile_data) {
		delete[] tile_data;
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
	if (tile_data != nullptr)
		delete[] tile_data;
	tile_data = new TILE_DATA_TYPE[size.x * size.y];
	memset(tile_data, 0, sizeof(TILE_DATA_TYPE) * size.x * size.y);

	unsigned char* tex = new unsigned char[size.x * size.y * 3];
	memset(tex, 0, sizeof(unsigned char) * size.x * size.y * 3);

	oglh::GenTextureData(id_tex, oglh::Wrap::Repeat, oglh::Filter::Nearest, size.x, size.y, tex);

	delete[] tex;
}

void MapLayerTile::Resize(const int2& oldSize, const int2& newSize)
{
	TILE_DATA_TYPE* new_data = new TILE_DATA_TYPE[newSize.x * newSize.y];
	memset(new_data, 0, sizeof(TILE_DATA_TYPE) * newSize.x * newSize.y);

	{
		PROFILE_SECTION("Copy data");
		for (int i = 0; i < oldSize.x * oldSize.y; ++i) {
			int2 colrow = int2(i % oldSize.x, (i / oldSize.x));
			if (colrow.x < newSize.x && colrow.y < newSize.y) {
				int new_index = (colrow.x + newSize.x * colrow.y);
				int old_index = (colrow.x + oldSize.x * colrow.y);
				new_data[new_index] = tile_data[old_index];
			}
		}
	}

	{
		PROFILE_SECTION("Gen Texture");
		delete[] tile_data;
		tile_data = new_data;

		unsigned char* tex = new unsigned char[newSize.x * newSize.y * 3];
		memset(tex, 0, sizeof(unsigned char) * newSize.x * newSize.y * 3);

		for (auto i = 0; i < newSize.x * newSize.y; ++i) {
			if (new_data[i] != 0) {
				tex[i * 3] = (unsigned char)(new_data[i] / UCHAR_MAX);
				tex[i * 3 + 1] = 0;
				tex[i * 3 + 2] = (unsigned char)(new_data[i] % UCHAR_MAX);
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

std::string MapLayerTile::Parse(int sizeX, int sizeY, DataTypeExport d) const
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

nlohmann::json MapLayerTile::Parse(int sizeX, int sizeY) const
{
	nlohmann::json ret;

	for (int i = sizeY - 1; i >= 0; --i) {
		for (int j = 0; j < sizeX; ++j) {
			ret.push_back(tile_data[i * sizeX + j]); // TODO: encode 4 bytes array
		}
	}

	return ret;
}

void MapLayerTile::Unparse(int sizeX, int sizeY, const std::string& raw_data)
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

nlohmann::json MapLayerTile::Serialize(const int2& size) const
{
	nlohmann::json lay = MapLayer::Serialize(size);

	lay["encoding"] = "base64-zlib";
	lay["data"] = Parse(size.x, size.y, MapLayer::DataTypeExport::BASE64_ZLIB);

	return lay;
}

void MapLayerTile::Deserialize(const nlohmann::json& json, const int2& size)
{
	MapLayer::Deserialize(json, size);

	Unparse(size.x, size.y, json.value("data", "0")/*TODO: unparse type (csv, base64, zlib...)*/);

	unsigned char* tex_data = new unsigned char[size.x * size.y * 3];
	memset(tex_data, 254, sizeof(unsigned char) * size.x * size.y * 3);

	for (int i = 0; i < size.x * size.y; ++i) {
		if (tile_data[i] != 0) {
			tex_data[i * 3] = (unsigned char)(tile_data[i] / UCHAR_MAX);
			tex_data[i * 3 + 1] = 0;
			tex_data[i * 3 + 2] = (unsigned char)(tile_data[i] % UCHAR_MAX);
		}
	}

	oglh::GenTextureData(id_tex, oglh::Wrap::Repeat, oglh::Filter::Nearest, size.x, size.y, tex_data);
	oglh::UnBindTexture();
	delete[] tex_data;
}
