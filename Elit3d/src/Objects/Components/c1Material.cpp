#include "Objects/Components/c1Material.h"

#include "Core/Application.h"
#include "Modules/m1Render3D.h"
#include "Modules/m1Resources.h"
#include "Resources/r1Texture.h"
#include "Resources/r1Shader.h"

#include "Tools/OpenGL/OpenGLHelper.h"

#include "ExternalTools/ImGui/imgui.h"

#include "ExternalTools/mmgr/mmgr.h"

c1Material::c1Material(Object* obj) : Component(obj, Type::Material)
{
	shader = App->render->GetShader("default");
}

c1Material::~c1Material()
{
}

bool c1Material::BindTex()
{
	if (tex != 0ULL) {
		auto t = (r1Texture*)App->resources->Get(tex);
		if (t != nullptr) {
			oglh::BindTexture(t->GetBufferID());
			return true;
		}
	}
	return false;
}

void c1Material::UnBindTex()
{
	oglh::UnBindTexture();
}

void c1Material::OnInspector()
{
	static bool choose_texture = false;
	if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen)) {
		if (tex != 0u) {
			auto t = (r1Texture*)App->resources->Get(tex);
			if (t != nullptr) {
				ImGui::Text("Texture: "); ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.f, 0.6f, 0.f, 1.f), "%s", t->name.c_str());

				ImGui::Text("UID: "); ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.f, 0.6f, 0.f, 1.f), "%s", std::to_string(t->GetUID()).c_str());

				ImGui::Text("Referneces: "); ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.f, 0.6f, 0.f, 1.f), "%u", t->references);

				ImGui::Image((void*)(intptr_t)(t->GetBufferID()), ImVec2(200, 200), ImVec2(0, 0), ImVec2(1, -1));
				if (ImGui::Button("Change texture")) {
					if (!choose_texture) {
						choose_texture = true;
						auto vec = App->resources->GetVectorOf(Resource::Type::Texture);
						for (auto i = vec.begin(); i != vec.end(); ++i) {
							if ((*i)->GetUID() != tex)
								((r1Texture*)(*i))->Attach();
						}
					}
				}

				if (choose_texture) {
					ChooseTextureWindow(choose_texture);
				}
			}
		}
	}
}

void c1Material::SetTexture(const uint64_t& id)
{
	tex = id;
	App->resources->Get(tex)->Attach();
}

void c1Material::SetTexture(const r1Texture* tex)
{
	this->tex = tex->uid;
	App->resources->Get(this->tex)->Attach();
}

void c1Material::SetTexture(const char* name_tex)
{
	tex = App->resources->FindByName(name_tex);
	App->resources->Get(tex)->Attach();
}

nlohmann::json c1Material::Parse()
{
	nlohmann::json ret = Component::Parse();

	ret["color"]["r"] = color[0];
	ret["color"]["g"] = color[1];
	ret["color"]["b"] = color[2];

	ret["texture_uid"] = tex;

	ret["shader"] = shader->GetIdentifier();

	return ret;
}

void c1Material::Unparse(const nlohmann::json& node)
{
	color.x = node["color"].value("r", 0.f);
	color.y = node["color"].value("g", 0.f);
	color.z = node["color"].value("b", 0.f);

	tex = node.value("texture_uid", 0ULL);
	if (auto r = App->resources->Get(tex))
		r->Attach();

	if (std::string nShader = node.value("shader", "default"); nShader != "default")
		shader = App->render->GetShader(nShader.c_str());
}

void c1Material::ChooseTextureWindow(bool& choose_texture)
{
	bool window = ImGui::Begin("Choose Texture", &choose_texture);

	if (!choose_texture) {
		auto vec = App->resources->GetVectorOf(Resource::Type::Texture);
		for (auto i = vec.begin(); i != vec.end(); ++i) {
			if ((*i)->GetUID() != tex)
				((r1Texture*)(*i))->Detach();
		}
	}

	if (window) {
		auto vec = App->resources->GetVectorOf(Resource::Type::Texture);

		for (auto i = vec.begin(); i < vec.end(); ++i) {
			if ((*i)->GetUID() != tex && ImGui::ImageButton((ImTextureID)((r1Texture*)(*i))->GetBufferID(), ImVec2(120, 120), ImVec2(0, 0), ImVec2(1, -1))) {
				(*i)->Attach();
				tex = (*i)->GetUID();
				choose_texture = false;
				for (auto j = vec.begin(); j != vec.end(); ++j) {
					((r1Texture*)(*j))->Detach();
				}
			}
		}
		ImGui::End();
	}
}
