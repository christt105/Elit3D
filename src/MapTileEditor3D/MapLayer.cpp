#include "MapLayer.h"

#include "Application.h"
#include "m1Render3D.h"
#include "r1Shader.h"

#include "ExternalTools/MathGeoLib/include/Math/Quat.h"

#include "ExternalTools/ImGui/IconsFontAwesome5/IconsFontAwesome5.h"

#include "OpenGLHelper.h"
#include "TypeVar.h"
#include "Profiler.h"

#include "ExternalTools/mmgr/mmgr.h"

OpenGLBuffers Layer::tile = OpenGLBuffers();

Layer::Layer()
{
	if (tile.vertices.size == 0u)
		tile.InitData();
	strcpy_s(buf, 30, name.c_str());
}

Layer::~Layer()
{
	if (tile_data) {
		delete[] tile_data;
		oglh::DeleteTexture(id_tex);
	}

	for (auto p : properties) {
		delete p.second;
	}
	properties.clear();
}

void Layer::Draw(const int2& size, int tile_width, int tile_height) const
{
	PROFILE_FUNCTION();
	
	oglh::BindTexture(id_tex);

	static auto shader = App->render->GetShader("tilemap");
	shader->SetMat4("model", float4x4::FromTRS(float3((float)displacement[0] / (float)tile_width, height, (float)displacement[1] / (float)tile_height), Quat::identity, float3((float)size.x, 1.f, (float)size.y))); // TODO: don't create a mat4x4 for every layer
	shader->SetFloat("alpha", opacity);
	oglh::DrawElements(tile.indices.size);
}

void Layer::Reset(const int2& size)
{
	tile_data = new TILE_DATA_TYPE[size.x * size.y];
	memset(tile_data, 0, sizeof(TILE_DATA_TYPE) * size.x * size.y);

	unsigned char* tex = new unsigned char[size.x * size.y * 3];
	memset(tex, 0, sizeof(unsigned char) * size.x * size.y * 3);

	oglh::GenTextureData(id_tex, oglh::Wrap::Repeat, oglh::Filter::Nearest, size.x, size.y, tex);

	delete[] tex;
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
	ImGui::Checkbox("Visible", &visible);
	ImGui::Checkbox("Lock", &locked);
	ImGui::DragFloat("Height", &height, 0.1f);
	ImGui::SliderFloat("Opacity", &opacity, 0.f, 1.f);
	ImGui::DragInt2("Displacement", displacement);

	ImGui::Separator();

	if (ImGui::CollapsingHeader("Custom Properties", ImGuiTreeNodeFlags_DefaultOpen)) {
		CreateProperty();
		ImGui::Separator();
		DisplayProperties();
	}
}

void Layer::DisplayProperties()
{
	if (ImGui::BeginChild("##properties")) {
		for (auto i = properties.begin(); i != properties.end(); ++i) {
			ImGui::PushID((*i).second);
			if (ImGui::Button(ICON_FA_TRASH_ALT)) {
				delete (*i).second;
				properties.erase(i);
				ImGui::PopID();
				break;
			}
			ImGui::SameLine();
			switch ((*i).second->type)
			{
			case TypeVar::Type::String:
				char b[30];
				strcpy_s(b, 30, static_cast<sTypeVar*>((*i).second)->value.c_str());
				if (ImGui::InputText((*i).first.c_str(), b, 30))
					static_cast<sTypeVar*>((*i).second)->value.assign(b);
				break;
			case TypeVar::Type::Int:
				ImGui::InputInt((*i).first.c_str(), &static_cast<iTypeVar*>((*i).second)->value);
				break;
			case TypeVar::Type::Float:
				ImGui::InputFloat((*i).first.c_str(), &static_cast<fTypeVar*>((*i).second)->value);
				break;
			case TypeVar::Type::Bool:
				ImGui::Checkbox((*i).first.c_str(), &static_cast<bTypeVar*>((*i).second)->value);
				break;
			default:
				break;
			}
			ImGui::PopID();
		}
		ImGui::EndChild();
	}
}

void Layer::CreateProperty()
{
	static char buffer[30] = { "" };
	static const char* types[4] = { "Int", "String", "Float", "Bool" };
	static int selected = 0;
	ImGui::PushID("##properties name");
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
	ImGui::InputText("Name", buffer, 30);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.25f);
	if (ImGui::BeginCombo("Type", types[selected])) {
		for (int i = 0; i < 4; ++i)
			if (ImGui::Selectable(types[i], i == selected))
				selected = i;
		ImGui::EndCombo();
	}
	ImGui::SameLine();
	if (ImGui::Button(ICON_FA_PLUS)) {
		if (!std::string(buffer).empty()) {
			if (properties.find(buffer) == properties.end()) {
				switch (selected)
				{
				case 0:
					properties[buffer] = new iTypeVar(0);
					break;
				case 1:
					properties[buffer] = new sTypeVar();
					break;
				case 2:
					properties[buffer] = new fTypeVar(0.f);
					break;
				case 3:
					properties[buffer] = new bTypeVar(false);
					break;
				default:
					break;
				}
			}
			strcpy_s(buffer, 30, "");
		}
	}
	ImGui::PopID();
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
