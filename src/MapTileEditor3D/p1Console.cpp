#include "p1Console.h"

#include "Application.h"

#include "ExternalTools/ImGui/IconsFontAwesome5/IconsFontAwesome5.h"

p1Console::p1Console(bool start_enabled) : Panel("Console", start_enabled)
{
	flags = ImGuiWindowFlags_MenuBar;
}

p1Console::~p1Console()
{
	for (auto i = logs.begin(); i != logs.end(); ++i) {
		delete* i;
	}
}

void p1Console::Update()
{
	static int count = 0;
	if (count == 0)
		LOG("LOOOOOOOOL XD");
	LOG("%i %.4f", count++, App->GetDt());
	LOGW("%i %.4f", count++, App->GetDt());
	LOGE("%i %.4f", count++, App->GetDt());

	if (ImGui::BeginMenuBar()) {
		if (ImGui::Selectable("Collapse", collapse, ImGuiSelectableFlags_None, ImVec2(65, 50)))
			collapse = !collapse;

		if (ImGui::Button("Clear")) {
			for (auto i = logs.begin(); i != logs.end(); ++i) {
				delete* i;
			}
			ImGui::SetScrollHereY(0.f);
			logs.clear();
			map_logs.clear();
		}

		if (ImGui::Checkbox("AutoScroll", &autoscroll)) {
			/*if(autoscroll)
				ImGui::SetScrollHereY(1.0f);*/
		}

		ImGui::EndMenuBar();
	}

	static bool pushed = false;
	static ImGuiSelectableFlags flags = ImGuiSelectableFlags_AllowItemOverlap;
	if (collapse) {
		for (auto i = map_logs.begin(); i != map_logs.end(); ++i) {
			ImGui::PushID(((*i).second));

			if ((*i).second->references > 1U)
				flags = ImGuiSelectableFlags_AllowItemOverlap;
			else
				flags = ImGuiSelectableFlags_AllowItemOverlap | ImGuiSelectableFlags_Disabled;

			ImGui::Selectable("## log", &(*i).second->opened, flags, ImVec2(0, 20));
			ImGui::AlignTextToFramePadding();

			ImGui::SameLine();

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4.f);

			switch ((*i).second->type)
			{
			case Logger::LogType::WARNING:
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.6f, 0.f, 1.f));
				ImGui::Text(ICON_FA_EXCLAMATION_TRIANGLE " \t");
				pushed = true;
				break;
			case Logger::LogType::ERR:
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.f, 0.f, 1.f));
				ImGui::Text(ICON_FA_EXCLAMATION_CIRCLE " \t");
				pushed = true;
				break;
			default:
				ImGui::Text(" " ICON_FA_INFO " \t");
				pushed = false;
				break;
			}
			ImGui::SameLine();
			ImGui::SetCursorPosX(40.f);

			ImGui::PopID();
			ImGui::Text(((*i).second)->last_line.c_str());

			ImGui::SameLine();

			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 25.f);
			ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 1.f), std::to_string((*i).second->references).c_str());

			if((*i).second->opened){
				ImGui::Indent(45.f);
				ImGui::TextUnformatted((*i).second->buffer.c_str());
				ImGui::Unindent(45.f);
			}
			if(pushed)
				ImGui::PopStyleColor();
			ImGui::Separator();
		}
	}
	else {
		for (auto i = logs.begin(); i != logs.end(); ++i) {
			ImGui::AlignTextToFramePadding();

			switch ((*i)->type)
			{
			case Logger::LogType::WARNING:
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.6f, 0.f, 1.f));
				ImGui::Text(ICON_FA_EXCLAMATION_TRIANGLE " \t");
				pushed = true;
				break;
			case Logger::LogType::ERR:
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.f, 0.f, 1.f));
				ImGui::Text(ICON_FA_EXCLAMATION_CIRCLE " \t");
				pushed = true;
				break;
			default:
				ImGui::Text(" " ICON_FA_INFO " \t");
				pushed = false;
				break;
			}
			ImGui::SameLine();
			ImGui::SetCursorPosX(40.f);
			ImGui::TextUnformatted((*i)->identifier.c_str());

			if (pushed)
				ImGui::PopStyleColor();
			ImGui::Separator();
		}
	}

	if (autoscroll/* && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()*/)
		ImGui::SetScrollHereY(1.0f);
}

