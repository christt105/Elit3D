#include "Panels/p1Console.h"

#include "Core/Application.h"

#include "ExternalTools/ImGui/IconsFontAwesome5/IconsFontAwesome5.h"

p1Console::p1Console(bool start_enabled, bool appear_mainmenubar, bool can_close)
	: Panel("Console", start_enabled, appear_mainmenubar, can_close, ICON_FA_ALIGN_LEFT)
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

		ImGui::Checkbox("AutoScroll", &autoscroll);

		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 150.f);
		if (ImGui::Selectable(" " ICON_FA_INFO, info, ImGuiSelectableFlags_None, ImVec2(20.f, 50.f)))
			info = !info;
		ImGui::Spacing();
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.6f, 0.f, 1.f));
		if (ImGui::Selectable(ICON_FA_EXCLAMATION_TRIANGLE, warning, ImGuiSelectableFlags_None, ImVec2(15.f, 50.f)))
			warning = !warning;
		ImGui::PopStyleColor();
		ImGui::Spacing();
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.f, 0.f, 1.f));
		if (ImGui::Selectable(ICON_FA_EXCLAMATION_CIRCLE, error, ImGuiSelectableFlags_None, ImVec2(15.f, 50.f)))
			error = !error;
		ImGui::PopStyleColor();

		ImGui::EndMenuBar();
	}

	static bool pushed = false;
	static ImGuiSelectableFlags flags = ImGuiSelectableFlags_AllowItemOverlap;
	
	if (collapse) {
		for (auto i = map_logs.begin(); i != map_logs.end(); ++i) {
			ImGui::PushID(((*i).second));
			if ((*i).second->type == Logger::LogType::INFO && !info) {
				ImGui::PopID();
				continue;
			}
			if ((*i).second->type == Logger::LogType::WARNING && !warning) {
				ImGui::PopID();
				continue;
			}
			if ((*i).second->type == Logger::LogType::ERR && !error) {
				ImGui::PopID();
				continue;
			}

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

			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 5.f - ImGui::CalcTextSize(std::to_string((*i).second->references).c_str()).x);
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
		if (info && error && warning) {
			ImGuiListClipper clipper;
			clipper.Begin(logs.size());
			while (clipper.Step())
			{
				for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
				{
					ImGui::AlignTextToFramePadding();

					switch (logs[line_no]->type)
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
					ImGui::TextUnformatted(logs[line_no]->identifier.c_str());

					if (pushed)
						ImGui::PopStyleColor();
					ImGui::Separator();
				}
			}
			clipper.End();
		}
		else {
			for (auto i = logs.begin(); i != logs.end(); ++i)
			{
				if ((*i)->type == Logger::LogType::INFO && !info) {
					continue;
				}
				if ((*i)->type == Logger::LogType::WARNING && !warning) {
					continue;
				}
				if ((*i)->type == Logger::LogType::ERR && !error) {
					continue;
				}

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
	}

	if (autoscroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 9.f)
		ImGui::SetScrollHereY(1.0f);
}

