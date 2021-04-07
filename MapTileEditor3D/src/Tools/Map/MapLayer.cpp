#include "Tools/Map/MapLayer.h"

#include "Core/Application.h"
#include "Modules/m1Render3D.h"
#include "Resources/r1Shader.h"

#include "Modules/m1GUI.h"
#include "Panels/p1Terrain.h"
#include "Resources/r1Tileset3d.h"

#include "Modules/m1Objects.h"
#include "Modules/m1Resources.h"
#include "Resources/r1Object.h"
#include "Resources/r1Texture.h"
#include "Resources/r1Model.h"
#include "Resources/r1Mesh.h"

#include "ExternalTools/MathGeoLib/include/Math/Quat.h"

#include "ExternalTools/ImGui/IconsFontAwesome5/IconsFontAwesome5.h"

#include "Objects/Object.h"

#include "Tools/OpenGL/OpenGLHelper.h"
#include "Tools/TypeVar.h"
#include "Tools/System/Profiler.h"

#include "ExternalTools/base64/base64.h"
#include "ExternalTools/zlib/zlib_strings.h"

#include "ExternalTools/ImGui/imgui.h"

#include "ExternalTools/mmgr/mmgr.h"


OpenGLBuffers Layer::tile = OpenGLBuffers();

Layer::Layer(Layer::Type t) : type(t)
{
	if (tile.vertices.size == 0u)
		tile.InitData();
	strcpy_s(buf, 30, name.c_str());

	switch (t)
	{
	case Layer::Type::TILE:
		break;
	case Layer::Type::OBJECT:
		root = App->objects->CreateEmptyObject(nullptr, "root");
		break;
	case Layer::Type::TERRAIN:
		/*terrain_data.push_back(std::make_tuple(-1.f, nullptr));
		terrain_data.push_back(std::make_tuple( 0.f, nullptr));
		terrain_data.push_back(std::make_tuple( 1.f, nullptr));*/
		break;
	default:
		break;
	}
}

Layer::~Layer()
{
	switch (type)
	{
	case Layer::Type::TILE:
	case Layer::Type::TERRAIN:
		if (tile_data) {
			delete[] tile_data;
			if (type == Layer::Type::TILE)
				oglh::DeleteTexture(id_tex);
		}
		break;
	case Layer::Type::OBJECT:
		App->objects->DeleteObject(root);
		if (object_tile_data) {
			delete[] object_tile_data;
		}
		break;
	/*case Layer::Type::TERRAIN:
		for (auto i = terrain_data.begin(); i != terrain_data.end(); ++i) {
			if (std::get<1>(*i) != nullptr) {
				delete[] std::get<1>(*i);
			}
		}
		break;*/
	default:
		break;
	}
}

void Layer::Draw(const int2& size, int tile_width, int tile_height) const
{
	PROFILE_FUNCTION();

	switch (type)
	{
	case Layer::Type::TILE: {
		oglh::BindTexture(id_tex);

		static auto shader = App->render->GetShader("tilemap");
		shader->SetMat4("model", float4x4::FromTRS(float3((float)displacement[0] / (float)tile_width, height, (float)displacement[1] / (float)tile_height), Quat::identity, float3((float)size.x, 1.f, (float)size.y))); // TODO: don't create a mat4x4 for every layer
		shader->SetFloat("alpha", opacity);
		oglh::DrawElements(tile.indices.size);
		break; }
	case Layer::Type::OBJECT: {
		if (root != nullptr) {
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
		break; }
	case Layer::Type::TERRAIN: {
		oglh::DepthEnable(true);
		static auto shader = App->render->GetShader("default");
		shader->Use();
		/*for (auto& t : terrain_data)*/ {
			TILE_DATA_TYPE* layer = tile_data;
			if (layer != nullptr)
				//continue;
			for (int i = size.x * size.y - 1; i >= 0; --i) {
				if (layer[i] == 0U)
					continue;
				r1Tileset3d* tileset = App->gui->terrain->tileset;
				r1Tileset3d::Tile3d* tile = tileset->tiles[layer[i]-1];
				r1Model* obj = (r1Model*)App->resources->Get(tile->uidObject);
				if (obj == nullptr)
					continue;
				
				for (auto& m : obj->meshes) {
					oglh::BindBuffers(m->VAO, m->vertices.id, m->indices.id);
					
					shader->SetMat4("model",  float4x4::FromTRS(float3(i % size.x, height /*+ std::get<0>(t)*/, i / size.x), Quat::identity /*I know, I know, is ugly I will change it (TODO)*/, float3::one) * tile->transform.GetGlobalMatrix());
					
					auto tex = (r1Texture*)App->resources->Get(obj->materials[m->tex_i]);
					if (tex != nullptr) tex->Bind();
					shader->SetBool("useTexture", m->texture.data != nullptr);
					//shader->SetBool("useTexture", false);
					oglh::DrawElements(m->indices.size);
					if (tex != nullptr) tex->Unbind();
				}
			}
		}
		oglh::DepthEnable(false);
		break;
	}
	}
}

void Layer::Reset(const int2& size)
{
	switch (type)
	{
	case Layer::Type::TILE: {
		if (tile_data != nullptr)
			delete[] tile_data;
		tile_data = new TILE_DATA_TYPE[size.x * size.y];
		memset(tile_data, 0, sizeof(TILE_DATA_TYPE) * size.x * size.y);

		unsigned char* tex = new unsigned char[size.x * size.y * 3];
		memset(tex, 0, sizeof(unsigned char) * size.x * size.y * 3);

		oglh::GenTextureData(id_tex, oglh::Wrap::Repeat, oglh::Filter::Nearest, size.x, size.y, tex);

		delete[] tex;
		break;
	}
	case Layer::Type::OBJECT:
		root = App->objects->CreateEmptyObject();
		if (object_tile_data != nullptr)
			delete[] object_tile_data;
		object_tile_data = new uint64_t[size.x * size.y];
		memset(object_tile_data, 0, sizeof(uint64_t) * size.x * size.y);
		break;
	case Layer::Type::TERRAIN:
		if (tile_data != nullptr)
			delete[] tile_data;
		tile_data = new TILE_DATA_TYPE[size.x * size.y];
		memset(tile_data, 0, sizeof(TILE_DATA_TYPE) * size.x * size.y);
		/*for (auto& i : terrain_data) {
			if (std::get<1>(i) != nullptr)
				delete[] std::get<1>(i);
			std::get<1>(i) = new TILEOBJECT_DATA_TYPE[size.x * size.y];
			memset(std::get<1>(i), 0, sizeof(TILEOBJECT_DATA_TYPE) * size.x * size.y);
		}*/
		break;
	default:
		break;
	}
}

void Layer::SelectTex() const
{
	oglh::BindTexture(id_tex);
}

void Layer::SelectBuffers()
{
	oglh::BindBuffers(tile.VAO, tile.vertices.id, tile.indices.id);
}

void Layer::DrawTile(const int2& size)
{
	static auto shader = App->render->GetShader("tilemap");
	shader->SetMat4("model", float4x4::FromTRS(float3(0.f, 0.f, 0.f), Quat::identity, float3((float)size.x, 1.f, (float)size.y))/* height of layer */);
	oglh::DrawElements(tile.indices.size);
}

bool Layer::HeightOrder(const Layer* l1, const Layer* l2)
{
	return l1->height < l2->height;
}

void Layer::OnInspector()
{
	if (ImGui::InputText("Name", buf, 30))
		name.assign(buf);
	ImGui::Text("Type: "); ImGui::SameLine(); ImGui::Text(Layer::TypeToString(type).c_str());
	ImGui::Checkbox("Visible", &visible);
	ImGui::Checkbox("Lock", &locked);
	ImGui::SliderFloat("Opacity", &opacity, 0.f, 1.f);
	ImGui::DragFloat("Height", &height, 0.1f);
	ImGui::DragInt2("Displacement", displacement);

	ImGui::Separator();

	if (ImGui::CollapsingHeader("Custom Properties", ImGuiTreeNodeFlags_DefaultOpen)) {
		properties.Display();
	}
}

std::string Layer::Parse(int sizeX, int sizeY, DataTypeExport d) const
{
	std::string ret;

		if (d != Layer::DataTypeExport::CSV_NO_NEWLINE)
			ret = '\n';

		for (int i = sizeY - 1; i >= 0; --i) {
			for (int j = 0; j < sizeX; ++j) {
				if (type == Layer::Type::TILE) {
					ret.append(std::to_string(tile_data[i * sizeX + j]) + ','); // TODO: encode 4 bytes array
				}
				else if (type == Layer::Type::OBJECT) {
					ret.append(std::to_string(object_tile_data[i * sizeX + j]) + ','); // TODO: encode 4 bytes array
				}
			}

			if (i == 0)
				ret.pop_back();
			if (d != Layer::DataTypeExport::CSV_NO_NEWLINE)
				ret += '\n';
		}

		switch (d)
		{
		case Layer::DataTypeExport::BASE64_NO_COMPRESSION:
			ret = base64_encode(ret);
			break;
		case Layer::DataTypeExport::BASE64_ZLIB:
			ret = compress_string(ret);
			ret = base64_encode(ret);
			break;
		default:
			break;
		}

	return ret;
}

nlohmann::json Layer::Parse(int sizeX, int sizeY) const
{
	nlohmann::json ret;

	for (int i = sizeY - 1; i >= 0; --i) {
		for (int j = 0; j < sizeX; ++j) {
			ret.push_back(tile_data[i * sizeX + j]); // TODO: encode 4 bytes array
		}
	}

	return ret;
}

void Layer::Unparse(int sizeX, int sizeY, const std::string& raw_data)
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
			if (type == Type::TILE)
				tile_data[sizeX * y + x] = (TILE_DATA_TYPE)std::stoul(n);
			else if (type == Type::OBJECT) {
				uint64_t uid = std::stoull(n);
				if (uid != 0)
					if (Resource* res = App->resources->Get(uid)) res->Attach();
				object_tile_data[sizeX * y + x] = uid;
			}
			++x;
		}
		if (i != data.end())
			i++;
	}
}

const char* Layer::GetName() const
{
	return name.c_str();
}

void Layer::SetName(const char* n)
{
	name.assign(n);
	strcpy_s(buf, 30, n);
}

Layer::Type Layer::GetType() const
{
	return type;
}

void Layer::SetType(Type t)
{
	type = t;
	//TODO: delete all data if exist
}

std::string Layer::TypeToString(Type t)
{
	switch (t)
	{
	case Layer::Type::TILE:
		return std::string("tile");
		break;
	case Layer::Type::OBJECT:
		return std::string("object");
		break;
	case Layer::Type::TERRAIN:
		return std::string("terrain");
		break;
	}
	return std::string("NONE");
}

Layer::Type Layer::StringToType(const std::string& s)
{
	if (s.compare("tile")) {
		return Layer::Type::TILE;
	}
	else if (s.compare("object")) {
		return Layer::Type::OBJECT;
	}
	else if (s.compare("terrain")) {
		return Layer::Type::TERRAIN;
	}
	return Layer::Type::NONE;
}

OpenGLBuffers::OpenGLBuffers()
{
}

OpenGLBuffers::~OpenGLBuffers()
{
	oglh::DeleteBuffer(vertices.id);
	oglh::DeleteBuffer(indices.id);
	oglh::DeleteBuffer(texture.id);

	delete[] vertices.data;
	delete[] indices.data;
	delete[] texture.data;
}

void OpenGLBuffers::InitData()
{
	vertices.size = 4;
	vertices.data = new float[vertices.size * 3];

	vertices.data[0] = 0.f;		vertices.data[1 ] = 0.f;		vertices.data[2 ] = 0.f;
	vertices.data[3] = 1.f; 	vertices.data[4 ] = 0.f;		vertices.data[5 ] = 0.f;
	vertices.data[6] = 1.f; 	vertices.data[7 ] = 0.f;		vertices.data[8 ] = 1.f;
	vertices.data[9] = 0.f;		vertices.data[10] = 0.f;		vertices.data[11] = 1.f;

	indices.size = 6;
	indices.data = new unsigned int[indices.size];

	indices.data[0] = 0u; indices.data[1] = 2u; indices.data[2] = 1u;
	indices.data[3] = 0u; indices.data[4] = 3u; indices.data[5] = 2u;

	texture.size = 4;
	texture.data = new float[texture.size * 2];
	memset(texture.data, 0, texture.size * 2 * sizeof(float));

	texture.data[0] = 0.f;		texture.data[1] = 0.f;
	texture.data[2] = 1.f;		texture.data[3] = 0.f;
	texture.data[4] = 1.f;		texture.data[5] = 1.f;
	texture.data[6] = 0.f;		texture.data[7] = 1.f;

	// VERTEX ARRAY OBJECT
	oglh::GenVAO(VAO);

	// VERTICES BUFFER
	oglh::GenArrayBuffer(vertices.id, vertices.size, sizeof(float), 3, vertices.data, 0, 3);

	// TEXTURE COORDS BUFFER
	oglh::GenArrayBuffer(texture.id, texture.size, sizeof(float), 2, texture.data, 1, 2);

	// INDICES BUFFER
	oglh::GenElementBuffer(indices.id, indices.size, indices.data);

	oglh::UnBindBuffers();
}
