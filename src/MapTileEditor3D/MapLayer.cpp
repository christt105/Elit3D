#include "MapLayer.h"

#include "Application.h"
#include "m1Render3D.h"
#include "r1Shader.h"

#include "ExternalTools/MathGeoLib/include/Math/Quat.h"

#include "OpenGLHelper.h"

#include "Profiler.h"

#include "ExternalTools/mmgr/mmgr.h"

OpenGLBuffers Layer::tile = OpenGLBuffers();

Layer::Layer()
{
	if (tile.vertices.size == 0u)
		tile.InitData();
}

Layer::~Layer()
{
	if (tile_data) {
		delete[] tile_data;
		oglh::DeleteTexture(id_tex);
	}
}

void Layer::Prepare() const
{
	oglh::BindTexture(id_tex);
}

void Layer::Update(const int2& size) const
{
	PROFILE_FUNCTION();

	static auto shader = App->render->GetShader("tilemap");
	shader->SetMat4("model", float4x4::FromTRS(float3(0.f, height, 0.f), Quat::identity, float3((float)size.x, 1.f, (float)size.y))); // TODO: don't create a mat4x4 for every layer
	shader->SetFloat("alpha", opacity);
	oglh::DrawElements(tile.indices.size);
}

void Layer::Reset(const int2& size)
{
	tile_data = new unsigned char[size.x * size.y * 3];

	for (int i = 0; i < size.x * size.y; ++i) {
		tile_data[i*3    ] = 0;
		tile_data[i*3 + 1] = 255;
		tile_data[i*3 + 2] = 0;
	}

	oglh::GenTextureData(id_tex, true, true, size.x, size.y, tile_data);
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
	ImGui::Text(name.c_str());
	ImGui::Checkbox("Visible", &visible);
	ImGui::Checkbox("Lock", &locked);
	ImGui::DragFloat("Height", &height, 0.1f);
	ImGui::SliderFloat("Opacity", &opacity, 0.f, 1.f);
}

const char* Layer::GetName() const
{
	return name.c_str();
}

void Layer::SetName(const char* n)
{
	name.assign(n);
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
