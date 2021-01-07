#include "Properties.h"
#include "Tools/TypeVar.h"

#include "ExternalTools/ImGui/imgui.h"
#include "ExternalTools/ImGui/IconsFontAwesome5/IconsFontAwesome5.h"

Properties::Properties()
{
}

Properties::~Properties()
{
	Clear();
}

void Properties::Display()
{
	CreateProperty();
	ImGui::Separator();
	DisplayProperties();
}

void Properties::CreateProperty()
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

void Properties::DisplayProperties()
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
			static char changeNameBuffer[30];
			static auto iterator = i;
			static const char* types[4] = { "Int", "String", "Float", "Bool" };
			static int selected = 0;
			if (ImGui::Button(ICON_FA_PEN)) {
				//Popup window
				ImGui::OpenPopup("Edit Property Name");
				strcpy_s(changeNameBuffer, sizeof(char) * 30, (*i).first.c_str());
				iterator = i;
				selected = (int)(*i).second->type;

				ImGui::SetNextWindowCentered(ImVec2(250.f, 150.f));
			}
			if (ImGui::BeginPopupModal("Edit Property Name")) {
				ImGui::Text("Name:");
				ImGui::InputText("", changeNameBuffer, 30);

				if (ImGui::BeginCombo("Type", types[selected])) {
					for (int i = 0; i < 4; ++i)
						if (ImGui::Selectable(types[i], i == selected))
							selected = i;
					ImGui::EndCombo();
				}

				ImGui::NewLine();

				if (ImGui::Button("Accept")) {
					auto find = properties.find(std::string(changeNameBuffer));
					if ((*iterator).second->type != (TypeVar::Type)selected) {
						delete (*iterator).second;
						switch ((TypeVar::Type)selected)
						{
						case TypeVar::Type::Int:
							(*iterator).second = new iTypeVar(0);
							break;
						case TypeVar::Type::Float:
							(*iterator).second = new fTypeVar(0.f);
							break;
						case TypeVar::Type::String:
							(*iterator).second = new sTypeVar();
							break;
						case TypeVar::Type::Bool:
							(*iterator).second = new bTypeVar(false);
							break;
						default:
							break;
						}
					}
					if (find == iterator) { // Not changed name
						ImGui::CloseCurrentPopup();
					}
					else {
						if (find != properties.end()) {
							delete (*find).second;
							properties.erase(find);
						}
						auto in = properties.extract(iterator);
						in.key() = changeNameBuffer;
						properties.insert(std::move(in));
						ImGui::CloseCurrentPopup();
						ImGui::EndPopup();
						ImGui::PopID();
						break;
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel")) {
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
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

void Properties::SaveProperties(nlohmann::json& propParent)
{
	for (auto p = properties.begin(); p != properties.end(); ++p) {
		nlohmann::json prop = nlohmann::json::object();
		prop["name"] = (*p).first;
		prop["type"] = (*p).second->type;
		switch ((*p).second->type)
		{
		case TypeVar::Type::Int:
			prop["value"] = static_cast<iTypeVar*>((*p).second)->value;
			break;
		case TypeVar::Type::String:
			prop["value"] = static_cast<sTypeVar*>((*p).second)->value;
			break;
		case TypeVar::Type::Float:
			prop["value"] = static_cast<fTypeVar*>((*p).second)->value;
			break;
		case TypeVar::Type::Bool:
			prop["value"] = static_cast<bTypeVar*>((*p).second)->value;
			break;
		default:
			break;
		}
		propParent.push_back(prop);
	}
}

void Properties::SaveProperties(pugi::xml_node& propParent)
{
	for (auto p = properties.begin(); p != properties.end(); ++p) {
		pugi::xml_node prop = propParent.append_child("property");
		prop.append_attribute("name").set_value((*p).first.c_str());
		prop.append_attribute("type").set_value(TypeVar::TypeToName((*p).second->type).c_str());
		switch ((*p).second->type)
		{
		case TypeVar::Type::Int:
			prop.append_attribute("value").set_value(static_cast<iTypeVar*>((*p).second)->value);
			break;
		case TypeVar::Type::String:
			prop.append_attribute("value").set_value(static_cast<sTypeVar*>((*p).second)->value.c_str());
			break;
		case TypeVar::Type::Float:
			prop.append_attribute("value").set_value(static_cast<fTypeVar*>((*p).second)->value);
			break;
		case TypeVar::Type::Bool:
			prop.append_attribute("value").set_value(static_cast<bTypeVar*>((*p).second)->value);
			break;
		default:
			break;
		}
	}
}

void Properties::LoadProperties(nlohmann::json& propParent)
{
	for (auto p = propParent.begin(); p != propParent.end(); ++p) {
		switch ((TypeVar::Type)(*p).value("type", 0))
		{
		case TypeVar::Type::Int:
			properties[(*p).value("name", "UNKNOWN")] = new iTypeVar((*p).value("value", 0));
			break;
		case TypeVar::Type::Float:
			properties[(*p).value("name", "UNKNOWN")] = new fTypeVar((*p).value("value", 0.f));
			break;
		case TypeVar::Type::Bool:
			properties[(*p).value("name", "UNKNOWN")] = new bTypeVar((*p).value("value", false));
			break;
		case TypeVar::Type::String:
			properties[(*p).value("name", "UNKNOWN")] = new sTypeVar((*p).value("value", std::string()));
			break;
		default:
			break;
		}
	}
}

void Properties::LoadProperties(pugi::xml_node& propParent)
{
}

void Properties::Clear()
{
	for (auto i = properties.begin(); i != properties.end(); ++i) {
		delete (*i).second;
	}
	properties.clear();
}
