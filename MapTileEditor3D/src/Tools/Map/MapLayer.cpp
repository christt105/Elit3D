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


OpenGLBuffers MapLayer::tile = OpenGLBuffers();

MapLayer::MapLayer(MapLayer::Type t) : type(t)
{
	if (tile.vertices.size == 0u)
		tile.InitData();
	strcpy_s(buf, 30, name.c_str());
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

nlohmann::json MapLayer::Serialize(const int2& size) const
{
	nlohmann::json lay = nlohmann::json::object();

	properties.SaveProperties(lay["properties"]);

	lay["name"] = name;
	lay["height"] = height;
	lay["opacity"] = opacity;
	lay["visible"] = visible;
	lay["locked"] = locked;
	lay["displacement"] = { displacement[0], displacement[1] };

	lay["type"] = type;

	return lay;
}

void MapLayer::Deserialize(const nlohmann::json& json, const int2& size)
{
	name	= json.value("name", "Layer");
	height	= json.value("height", 0.f);
	opacity = json.value("opacity", 1.f);
	visible = json.value("visible", true);
	locked	= json.value("locked", false);
	if (json.find("displacement") != json.end()) {
		displacement[0] = json["displacement"][0];
		displacement[1] = json["displacement"][1];
	}

	properties.LoadProperties(json["properties"]);
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
	case MapLayer::Type::TERRAIN:
		return std::string("terrain");
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
	case MapLayer::Type::TERRAIN:
		return "terrain";
	}
	return std::string("NONE");
}

MapLayer::Type MapLayer::StringToType(const std::string& s)
{
	if (s.compare("tile")) {
		return MapLayer::Type::TILE;
	}
	else if (s.compare("object")) {
		return MapLayer::Type::OBJECT;
	}
	else if (s.compare("terrain")) {
		return MapLayer::Type::TERRAIN;
	}
	return MapLayer::Type::NONE;
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
