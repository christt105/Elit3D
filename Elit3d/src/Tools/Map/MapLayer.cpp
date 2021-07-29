#include "Tools/Map/MapLayer.h"

#include "Core/Application.h"
#include "Modules/m1Render3D.h"
#include "Resources/r1Shader.h"

#include "Modules/m1GUI.h"
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

#include "Resources/r1Map.h"

#include "ExternalTools/ImGui/imgui.h"

#include "ExternalTools/mmgr/mmgr.h"


OpenGLBuffers MapLayer::tile = OpenGLBuffers();

MapLayer::MapLayer(MapLayer::Type t, r1Map* m) : map(m), type(t)
{
	if (tile.vertices.size == 0u)
		tile.InitData();
	strcpy_s(buf, 30, name.c_str());
}

void MapLayer::Resize(const int2& oldSize, const int2& newSize)
{
	TILE_DATA_TYPE* new_data = new TILE_DATA_TYPE[newSize.x * newSize.y];
	memset(new_data, 0, sizeof(TILE_DATA_TYPE) * newSize.x * newSize.y);

	for (int i = 0; i < oldSize.x * oldSize.y; ++i) {
		int2 colrow = int2(i % oldSize.x, (i / oldSize.x));
		if (colrow.x < newSize.x && colrow.y < newSize.y) {
			int new_index = (colrow.x + newSize.x * colrow.y);
			int old_index = (colrow.x + oldSize.x * colrow.y);
			new_data[new_index] = data[old_index];
		}
	}

	delete[] data;
	data = new_data;
}

void MapLayer::SelectBuffers()
{
	oglh::BindBuffers(tile.VAO, tile.vertices.id, tile.indices.id);
}

void MapLayer::DrawTile(const int2& size)
{
	static auto shader = App->render->GetShader("tilemap");
	shader->SetMat4("model", float4x4::FromTRS(float3(0.f, 0.f, 0.f), Quat::identity, float3((float)size.x, 1.f, (float)size.y))/* height of layer */);
	oglh::DrawElements(tile.indices.size);
}

bool MapLayer::HeightOrder(const MapLayer* l1, const MapLayer* l2)
{
	return l1->height < l2->height;
}

void MapLayer::OnInspector()
{
	if (ImGui::InputText("Name", buf, 30))
		name.assign(buf);
	ImGui::Text("Type: "); ImGui::SameLine(); ImGui::Text(MapLayer::TypeToString(type).c_str());
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

void MapLayer::Parse(pugi::xml_node& node, MapLayer::DataTypeExport t, bool exporting) const
{
	properties.SaveProperties(node.append_child("properties"));

	node.append_attribute("name").set_value(name.c_str());
	node.append_attribute("visible").set_value(visible);
	node.append_attribute("locked").set_value(locked);
	node.append_attribute("height").set_value(height);
	node.append_attribute("opacity").set_value(opacity);
	node.append_attribute("displacementx").set_value(displacement[0]);
	node.append_attribute("displacementy").set_value(displacement[1]);

	auto ndata = node.append_child("data");
	auto encoding = ndata.append_attribute("encoding");
	encoding.set_value(MapLayer::DataTypeToString(t).c_str());
	ndata.append_child(pugi::node_pcdata).set_value(SerializeData(t).c_str());

	node.append_attribute("type").set_value(TypeToString(type).c_str());
}

void MapLayer::Parse(nlohmann::json& node, MapLayer::DataTypeExport t, bool exporting) const
{
	properties.SaveProperties(node["properties"]);

	node["name"] = name;
	node["height"] = height;
	node["opacity"] = opacity;
	node["visible"] = visible;
	node["locked"] = locked;
	node["displacement"] = { displacement[0], displacement[1] };

	node["encoding"] = DataTypeToString(t);
	node["data"] = SerializeData(t);

	node["type"] = TypeToString(type);
}

std::string MapLayer::SerializeData(MapLayer::DataTypeExport t) const
{
	std::string ret;

	if (t != MapLayer::DataTypeExport::CSV_NO_NEWLINE)
		ret = '\n';

	int2 size = map->GetSize();
	for (int i = size.y - 1; i >= 0; --i) {
		for (int j = 0; j < size.x; ++j) {
			ret.append(std::to_string(data[i * size.x + j]) + ','); // TODO: encode 4 bytes array
		}

		if (i == 0)
			ret.pop_back();
		if (t != MapLayer::DataTypeExport::CSV_NO_NEWLINE)
			ret += '\n';
	}

	if (t == MapLayer::DataTypeExport::BASE64_NO_COMPRESSION)
		ret = base64_encode(ret);
	else if (t == MapLayer::DataTypeExport::BASE64_ZLIB)
		ret = base64_encode(compress_string(ret));

	return ret;
}

void MapLayer::DeserializeData(const std::string& strdata, MapLayer::DataTypeExport t) const
{
	std::string rawdata;
	switch (t)
	{
	case MapLayer::DataTypeExport::CSV:
	case MapLayer::DataTypeExport::CSV_NO_NEWLINE:
		rawdata = strdata;
		break;
	case MapLayer::DataTypeExport::BASE64_NO_COMPRESSION:
		rawdata = base64_decode(strdata);
		break;
	case MapLayer::DataTypeExport::BASE64_ZLIB:
		rawdata = decompress_string(base64_decode(strdata));
		break;
	}

	auto i = rawdata.begin();
	int2 size = map->GetSize();
	if (*i == '\n')
		++i;
	int x = 0;
	int y = size.y - 1;
	while (i != rawdata.end()) {
		std::string n;
		while (i != rawdata.end() && *i != ',') {
			if (*i == '\n' && (i + 1) != rawdata.end()) { // Weird way to load cause the origin on textures is Bottom-Left and not Top-Left. TODO?
				x = 0;
				--y;
				break;
			}
			n += *i;
			i++;
		}
		if (!n.empty()) {
			data[size.x * y + x] = (TILE_DATA_TYPE)std::stoul(n);
			++x;
		}
		if (i != rawdata.end())
			i++;
	}
}

void MapLayer::Unparse(const pugi::xml_node& node)
{
}

void MapLayer::Unparse(const nlohmann::json& node)
{
	name	= node.value("name", "Layer");
	height	= node.value("height", 0.f);
	opacity = node.value("opacity", 1.f);
	visible = node.value("visible", true);
	locked	= node.value("locked", false);

	if (node.find("displacement") != node.end()) {
		displacement[0] = node["displacement"][0];
		displacement[1] = node["displacement"][1];
	}

	properties.LoadProperties(node["properties"]);

	DeserializeData(node.value("data", ""), MapLayer::StringToDataType(node.value("encoding", "")));
}

const char* MapLayer::GetName() const
{
	return name.c_str();
}

void MapLayer::SetName(const char* n)
{
	name.assign(n);
	strcpy_s(buf, 30, n);
}

void MapLayer::SetSelected()
{
	strcpy_s(buf, 30, name.c_str());
}

MapLayer::Type MapLayer::GetType() const
{
	return type;
}

std::string MapLayer::TypeToString(Type t)
{
	switch (t)
	{
	case MapLayer::Type::TILE:
		return std::string("tile");
	case MapLayer::Type::OBJECT:
		return std::string("object");
	}
	return std::string("NONE");
}

std::string MapLayer::ToString() const
{
	switch (type)
	{
	case MapLayer::Type::TILE:
		return "tile";
	case MapLayer::Type::OBJECT:
		return "object";
	}
	return std::string("NONE");
}

MapLayer::Type MapLayer::StringToType(const std::string& s)
{
	if (s.compare("tile") == 0) {
		return MapLayer::Type::TILE;
	}
	else if (s.compare("object") == 0) {
		return MapLayer::Type::OBJECT;
	}
	return MapLayer::Type::NONE;
}

std::string MapLayer::DataTypeToString(DataTypeExport t)
{
	switch (t)
	{
	case MapLayer::DataTypeExport::CSV:
	case MapLayer::DataTypeExport::CSV_NO_NEWLINE:
		return "csv";
	case MapLayer::DataTypeExport::BASE64_NO_COMPRESSION:
		return "base64";
	case MapLayer::DataTypeExport::BASE64_ZLIB:
		return "base64-zlib";
	}

	return "none";
}

MapLayer::DataTypeExport MapLayer::StringToDataType(const std::string& s)
{
	if (s.compare("csv") == 0) {
		return MapLayer::DataTypeExport::CSV;
	}
	if (s.compare("base64") == 0) {
		return MapLayer::DataTypeExport::BASE64_NO_COMPRESSION;
	}
	if (s.compare("base64-zlib") == 0) {
		return MapLayer::DataTypeExport::BASE64_ZLIB;
	}

	return MapLayer::DataTypeExport::NONE;
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

MapLayer::EditLayerCommand::EditLayerCommand(MapLayer* layer) : lay(layer)
{
	data = layer->SerializeData(MapLayer::DataTypeExport::BASE64_ZLIB);
}

void MapLayer::EditLayerCommand::Undo()
{
	std::string tmp = lay->SerializeData(MapLayer::DataTypeExport::BASE64_ZLIB);
	lay->DeserializeData(data, MapLayer::DataTypeExport::BASE64_ZLIB);
	data = tmp;
	lay->SetDataAfterUnparse();
}

void MapLayer::EditLayerCommand::Redo()
{
	Undo();
}
