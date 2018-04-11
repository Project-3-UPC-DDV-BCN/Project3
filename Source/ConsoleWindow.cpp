#include "ConsoleWindow.h"
#include "Brofiler\Brofiler.h"

ConsoleWindow::ConsoleWindow()
{
	active = true;
	window_name = "Console";
	UpdateLabels();

	scroll_to_bottom = false;
	show_logs = true;
	show_warnings = true;
	show_errors = true;
	show_debug_logs = false;
	log_text_color = { 1.00f, 1.00f ,1.00f ,1.00f };
	warning_text_color = { 1.00f, 1.00f, 0.00f ,1.00f };
	error_text_color = { 1.00f, 0.00f, 0.00f ,1.00f };
	debug_text_color = { 0.40f, 0.90f, 0.90f ,1.00f };
	error_count = 0;
	warning_count = 0;
	log_count = 0;
	debug_count = 0;
}

ConsoleWindow::~ConsoleWindow()
{
}

void ConsoleWindow::DrawWindow()
{
	BROFILER_CATEGORY("Editor - Console: DrawWindow", Profiler::Color::Black);

	ImGui::SetNextWindowContentSize(ImGui::GetContentRegionAvail());
	if (ImGui::BeginDock(window_name.c_str(), false, false, false, ImGuiWindowFlags_HorizontalScrollbar)) {
		if (ImGui::Button(logs_label.c_str())) {
			show_logs = !show_logs;
		}
		ImGui::SameLine();
		if (ImGui::Button(warnings_label.c_str())) {
			show_warnings = !show_warnings;
		}
		ImGui::SameLine();
		if (ImGui::Button(errors_label.c_str())) {
			show_errors = !show_errors;
		}
		ImGui::SameLine();
		if (ImGui::Button(debug_label.c_str())) {
			show_debug_logs = !show_debug_logs;
		}
		ImGui::SameLine();
		if (ImGui::Button("Clear")) {
			Clear();
		}

		ImGui::Separator();
		ImGui::BeginChild("scrolling");

		for (std::map<MessageType, std::string>::iterator it = message_list.begin(); it != message_list.end(); it++)
		{
			switch (it->first)
			{
			case MessageLog:
				if (show_logs)
				{
					ImGui::TextColored(log_text_color, "%s", it->second.c_str());
				}
				break;
			case MessageWarning:
				if (show_warnings)
				{
					ImGui::TextColored(warning_text_color, "%s", it->second.c_str());
				}
				break;
			case MessageError:
				if (show_errors)
				{
					ImGui::TextColored(error_text_color, "%s", it->second.c_str());
				}
				break;
			case MessageDebug:
				if (show_debug_logs)
				{
					ImGui::TextColored(debug_text_color, "%s", it->second.c_str());
				}
				break;
			}
		}

		if (scroll_to_bottom)
			ImGui::SetScrollHere(1.0f);
		scroll_to_bottom = false;
		ImGui::EndChild();
	}
	ImGui::EndDock();
}

void ConsoleWindow::Clear()
{
	message_list.clear();
	error_count = 0;
	warning_count = 0;
	log_count = 0;
	debug_count = 0;
	UpdateLabels();
}

void ConsoleWindow::AddLog(std::string log, bool is_error, bool is_warning, bool is_debug)
{

	if (is_error) {
		if (error_count == 99) {
			std::map<MessageType, std::string>::iterator it = message_list.begin();
			while (it != message_list.end())
			{
				if (it->first == MessageError)
				{
					break;
				}
				else
				{
					it++;
				}
			}
			message_list.erase(it);
			error_count--;
		}

		if (error_count < 99) {
			message_list.insert(std::pair<MessageType, std::string>(MessageError, "[Error] " + log));
			error_count++;
		}
	}
	else if (is_warning) {
		if (warning_count == 99) {
			std::map<MessageType, std::string>::iterator it = message_list.begin();
			while (it != message_list.end())
			{
				if (it->first == MessageWarning)
				{
					break;
				}
				else
				{
					it++;
				}
			}
			message_list.erase(it);
			warning_count--;
		}

		if (warning_count < 99) {
			message_list.insert(std::pair<MessageType, std::string>(MessageWarning, "[Warning] " + log));
			warning_count++;
		}
	}
	else if (is_debug) {
		if (debug_count == 99) {
			std::map<MessageType, std::string>::iterator it = message_list.begin();
			while (it != message_list.end())
			{
				if (it->first == MessageDebug)
				{
					break;
				}
				else
				{
					it++;
				}
			}
			message_list.erase(it);
			debug_count--;
		}

		if (debug_count < 99) {
			message_list.insert(std::pair<MessageType, std::string>(MessageDebug, "[Debug] " + log));
			debug_count++;
		}
	}
	else {
		if (log_count == 99) {
			std::map<MessageType, std::string>::iterator it = message_list.begin();
			while (it != message_list.end())
			{
				if (it->first == MessageLog)
				{
					break;
				}
				else
				{
					it++;
				}
			}
			message_list.erase(it);
			log_count--;
		}
		
		if (log_count < 99)
		{
			message_list.insert(std::pair<MessageType, std::string>(MessageLog, "[Log] " + log));
			log_count++;
		}
	}

	scroll_to_bottom = true;
	UpdateLabels();
}

void ConsoleWindow::UpdateLabels()
{
	errors_label = "Errors (" + std::to_string(error_count) + ")";
	warnings_label = "Warnings (" + std::to_string(warning_count) + ")";
	logs_label = "Logs (" + std::to_string(log_count) + ")";
	debug_label = "Debug (" + std::to_string(debug_count) + ")";
}
